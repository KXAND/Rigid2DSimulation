#pragma once
#include "BodyDrag.h"

class AppInput;
class Body;
struct InputOwners;

class DragTool
{
public:
    double rotationSpeed = 0;

    BodyDrag const& GetDrag() const
    {
        return drag;
    }
    // 只解释输入；返回本次每帧作用的数据，首次按下返回空数据。
    BodyDrag UpdateBody(Body const& body, AppInput& input, InputOwners const& inputOwners,
                        double realDeltaTime, bool isHovered, bool enabled);
    void Cancel();
    // 删除前取消对待删除物体的引用，不负责删除物体。
    void PrepareForDeletion();

private:
    BodyDrag drag;

    void ReadDragInput(Body const& body, AppInput const& input, InputOwners const& inputOwners, double realDeltaTime);
};
