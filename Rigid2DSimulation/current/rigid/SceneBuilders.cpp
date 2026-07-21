#include "SceneBuilders.h"
#include "current/common/utils.h"
#include "SimulationApp.h"
#include "Background.h"
#include "Body.h"
#include "Shape.h"
#include "Connection.h"

void CreateBody(SimulationApp& simulationApp, Value const& shapeConfigurations, Value const& configuration)
{
	bool point = false;
	TryReadConfigNumber(configuration, L"point", point);
	std::shared_ptr<Body> body;
	vector<std::shared_ptr<Shape>> shapes;
	for (auto shapeConfig : shapeConfigurations.elements)
	{
		if (shapeConfig->elements.empty()) { continue; }
		std::shared_ptr<Shape> shape;
		if (shapeConfig->elements[0]->number)
		{
			if (shapeConfig->elements.size() < 2) { continue; }
			Vector2 relativePosition;
			if (shapeConfig->elements.size() >= 3) { relativePosition = ToVector2(*shapeConfig->elements[2]); }
			shape = std::make_shared<Shape>(shapeConfig->elements[1]->number, relativePosition);
		}
		else
		{
			if (shapeConfig->elements.size() < 2) { continue; }
			vector<Vector2> vertices;
			for (auto vertexValue : shapeConfig->elements[1]->elements)
			{
				vertices.push_back(ToVector2(*vertexValue));
			}
			shape = std::make_shared<Shape>(vertices);
		}
		shapes.push_back(shape);
	}
	if (point) { body = std::make_shared<Body>(Vector2()); }
	else { body = std::make_shared<Body>(shapes); }
	body->innerColor = Color(GenerateRandomInt(256), GenerateRandomInt(256), GenerateRandomInt(256));
	body->ReadConfig(configuration);
	bool resetPosition = false;
	TryReadConfigNumber(configuration, L"resetPosition", resetPosition);
	body->Init(resetPosition);
	simulationApp.bodies.push_back(body);
	simulationApp.isSceneChanged = true;
}
void CreateConnection(SimulationApp& simulationApp, Value const& configuration)
{
	auto& bodies = simulationApp.bodies;
	int idx0 = GenerateRandomInt(bodies.size());
	int idx1 = GenerateRandomInt(bodies.size());
	TryReadConfigNumber(configuration, L"index0", idx0);
	TryReadConfigNumber(configuration, L"index1", idx1);
	idx0 = Clamp(idx0, 0, (int)bodies.size());
	idx1 = Clamp(idx1, 0, (int)bodies.size());
	std::shared_ptr<Connection> connection;
	connection = std::make_shared<Connection>();
	connection->body0 = &*bodies[idx0];
	connection->body1 = &*bodies[idx1];
	connection->p0Relative = connection->body0->GenerateRandomPointInside();
	connection->p1Relative = connection->body1->GenerateRandomPointInside();
	bool absolute = false;
	TryReadConfigNumber(configuration, L"absolute", absolute);
	if (auto value = FindConfigValue(configuration, L"point0"))
	{
		connection->p0Relative = ToVector2(*value);
		if (absolute)
		{
			connection->p0Relative -= connection->body0->position;
			connection->p0Relative = connection->body0->transform.Inverse() * connection->p0Relative;
		}
	}
	if (auto value = FindConfigValue(configuration, L"point1"))
	{
		connection->p1Relative = ToVector2(*value);
		if (absolute)
		{
			connection->p1Relative -= connection->body1->position;
			connection->p1Relative = connection->body1->transform.Inverse() * connection->p1Relative;
		}
	}
	connection->SignUpToBodies();
	connection->UpdatePosition();
	connection->length = (connection->p0 - connection->p1).GetLength();
	connection->ReadConfig(configuration);
	simulationApp.connections.push_back(connection);
	simulationApp.isSceneChanged = true;
}

void Boundary(SimulationApp& simulationApp, Value const& thickness, Value const& configuration)
{
	double backgroundWidth = simulationApp.background->width, backgroundHeight = simulationApp.background->height;
	double left = simulationApp.background->topLeft.x, right = left + backgroundWidth;
	double top = simulationApp.background->topLeft.y, bottom = top + backgroundHeight;
	std::shared_ptr<Body> body;
	Color color = Color(GenerateRandomInt(256), GenerateRandomInt(256), GenerateRandomInt(256));
	body = std::make_shared<Body>(std::make_shared<Shape>(GetRectVerticesByCenter(thickness.number, backgroundHeight)));
	body->position = Vector2(left, (top + bottom) / 2);
	body->fixed = true;
	body->innerColor = color;
	body->ReadConfig(configuration);
	body->Init();
	simulationApp.bodies.push_back(body);
	body = std::make_shared<Body>(std::make_shared<Shape>(GetRectVerticesByCenter(thickness.number, backgroundHeight)));
	body->position = Vector2(right, (top + bottom) / 2);
	body->fixed = true;
	body->innerColor = color;
	body->ReadConfig(configuration);
	body->Init();
	simulationApp.bodies.push_back(body);
	body = std::make_shared<Body>(std::make_shared<Shape>(GetRectVerticesByCenter(backgroundWidth, thickness.number)));
	body->position = Vector2((left + right) / 2, top);
	body->fixed = true;
	body->innerColor = color;
	body->ReadConfig(configuration);
	body->Init();
	simulationApp.bodies.push_back(body);
	body = std::make_shared<Body>(std::make_shared<Shape>(GetRectVerticesByCenter(backgroundWidth, thickness.number)));
	body->position = Vector2((left + right) / 2, bottom);
	body->fixed = true;
	body->innerColor = color;
	body->ReadConfig(configuration);
	body->Init();
	simulationApp.bodies.push_back(body);
	simulationApp.isSceneChanged = true;
}
void Gear(SimulationApp& simulationApp, Value const& radius, Value const& toothCount, Value const& toothHeight, Value const& configuration)
{
	vector<std::shared_ptr<Shape>> shapes;
	shapes.push_back(std::make_shared<Shape>(radius.number));
	for (int i = (0), iEnd = int(toothCount.number); i < iEnd; ++i)
	{
		double phi0 = 2 * kPi * i / (int)toothCount.number;
		double phi1 = 2 * kPi * (i + 1) / (int)toothCount.number;
		double phi = (phi0 + phi1) / 2;
		Vector2 toothTip = Vector2(cos(phi), sin(phi)) * (radius.number + toothHeight.number);
		Vector2 v0 = Vector2(cos(phi0), sin(phi0)) * radius.number;
		Vector2 v1 = Vector2(cos(phi1), sin(phi1)) * radius.number;
		shapes.push_back(std::make_shared<Shape>(vector<Vector2>{ v0, toothTip, v1 }));
	}
	auto wheel = std::make_shared<Body>(shapes);
	wheel->innerColor = Color(GenerateRandomInt(256), GenerateRandomInt(256), GenerateRandomInt(256));
	wheel->ReadConfig(configuration);
	wheel->Init();
	simulationApp.bodies.push_back(wheel);
	auto root = std::make_shared<Body>(wheel->position);
	root->innerColor = wheel->innerColor;
	root->Init();
	simulationApp.bodies.push_back(root);
	auto connection = std::make_shared<Connection>();
	connection->type = kLinkConnection;
	connection->body0 = &*root;
	connection->body1 = &*wheel;
	connection->SignUpToBodies();
	connection->UpdatePosition();
	simulationApp.connections.push_back(connection);
	simulationApp.isSceneChanged = true;
}
void Strand(SimulationApp& simulationApp, Value const& point0Value, Value const& point1Value,
	Value const& countValue, Value const& ratio, Value const& bodyConfiguration, Value const& connectionConfiguration)
{
	Vector2 p0 = ToVector2(point0Value), p1 = ToVector2(point1Value);
	int bodyCount = countValue.number;
	std::shared_ptr<Body> previousBody, currentBody;
	std::shared_ptr<Connection> connection;
	double segmentLength = (p1 - p0).GetLength() / bodyCount;
	double bodyRadius = 0.5 * ratio.number * segmentLength;
	double gap = max(0.0, segmentLength - 2 * bodyRadius);
	Color bodyColor = Color(GenerateRandomInt(256), GenerateRandomInt(256), GenerateRandomInt(256));
	previousBody = std::make_shared<Body>(std::make_shared<Shape>(bodyRadius));
	previousBody->innerColor = bodyColor;
	previousBody->position = p0;
	previousBody->ReadConfig(bodyConfiguration);
	previousBody->Init();
	simulationApp.bodies.push_back(previousBody);
	for (int i = (1), iEnd = int(bodyCount + 1); i < iEnd; ++i)
	{
		currentBody = std::make_shared<Body>(std::make_shared<Shape>(bodyRadius));
		currentBody->innerColor = bodyColor;
		currentBody->position = (p0 * (bodyCount - i) + p1 * i) / bodyCount;
		currentBody->ReadConfig(bodyConfiguration);
		currentBody->Init();
		simulationApp.bodies.push_back(currentBody);
		connection = std::make_shared<Connection>();
		connection->body0 = &*previousBody;
		connection->body1 = &*currentBody;
		connection->length = gap;
		connection->p0Relative = +bodyRadius * (p1 - p0).GetNormalized();
		connection->p1Relative = -bodyRadius * (p1 - p0).GetNormalized();
		connection->ReadConfig(connectionConfiguration);
		connection->SignUpToBodies();
		connection->UpdatePosition();
		simulationApp.connections.push_back(connection);
		previousBody = currentBody;
	}
	simulationApp.isSceneChanged = true;
}
void Necklace(SimulationApp& simulationApp, Value const& centerValue, Value const& radius,
	Value const& countValue, Value const& ratio, Value const& bodyConfiguration, Value const& connectionConfiguration)
{
	std::shared_ptr<Body> firstBody, previousBody, currentBody;
	int bodyCount = countValue.number;
	Vector2 center = ToVector2(centerValue);
	std::shared_ptr<Connection> connection;
	double segmentLength = 2 * kPi * radius.number / bodyCount;
	double ballRadius = 0.5 * ratio.number * segmentLength;
	double gap = max(0.0, segmentLength - 2 * ballRadius);
	Color bodyColor = Color(GenerateRandomInt(256), GenerateRandomInt(256), GenerateRandomInt(256));
	previousBody = std::make_shared<Body>(std::make_shared<Shape>(ballRadius));
	previousBody->innerColor = bodyColor;
	previousBody->position = center + Vector2(radius.number, 0);
	previousBody->ReadConfig(bodyConfiguration);
	previousBody->Init();
	simulationApp.bodies.push_back(previousBody);
	firstBody = previousBody;
	for (int i = (1), iEnd = int(bodyCount); i < iEnd; ++i)
	{
		double phi = 2 * kPi * i / bodyCount;
		currentBody = std::make_shared<Body>(std::make_shared<Shape>(ballRadius));
		currentBody->innerColor = bodyColor;
		currentBody->position = center + Vector2(cos(phi), sin(phi)) * radius.number;
		currentBody->ReadConfig(bodyConfiguration);
		currentBody->Init();
		simulationApp.bodies.push_back(currentBody);
		connection = std::make_shared<Connection>();
		connection->body0 = &*previousBody;
		connection->body1 = &*currentBody;
		connection->length = gap;
		connection->p1Relative = ballRadius * Vector2(+sin(phi), -cos(phi));
		phi = 2 * kPi * (i - 1) / bodyCount;
		connection->p0Relative = ballRadius * Vector2(-sin(phi), +cos(phi));
		connection->ReadConfig(connectionConfiguration);
		connection->SignUpToBodies();
		connection->UpdatePosition();
		simulationApp.connections.push_back(connection);
		previousBody = currentBody;
	}
	double phi = 2 * kPi * bodyCount / bodyCount;
	currentBody = firstBody;
	connection = std::make_shared<Connection>();
	connection->body0 = &*previousBody;
	connection->body1 = &*currentBody;
	connection->length = gap;
	connection->p1Relative = ballRadius * Vector2(+sin(phi), -cos(phi));
	phi = 2 * kPi * (bodyCount - 1) / bodyCount;
	connection->p0Relative = ballRadius * Vector2(-sin(phi), +cos(phi));
	connection->ReadConfig(connectionConfiguration);
	connection->SignUpToBodies();
	connection->UpdatePosition();
	simulationApp.connections.push_back(connection);
	simulationApp.isSceneChanged = true;
}