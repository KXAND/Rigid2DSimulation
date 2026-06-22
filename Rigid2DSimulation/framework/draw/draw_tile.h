#pragma once
#include "vector2.h"
#include "tile.h"
/// <summary>
/// ��ײ��Tile������Background�����������
/// </summary>
/// <param name="dest"></param>
/// <param name="tl"></param>
/// <param name="src"></param>
void drawBasedTile(tile& dest, dVector2 tl, tile const& src);

/// <summary>
/// ������ȵĻ��� Tile����
/// ���� vp_dest ��������Ϊ0���쳣��������� vp_src ����������μ������ƻ�û��á�
/// </summary>
void drawTile(tile& dest, dbuf& ds, double dep, dVector2 tl, dRect vp_dest, tile const& src, dRect vp_src);
/// <summary>
/// ���ã�
/// </summary>
/// <param name="p"></param>
/// <param name="tl"></param>
/// <param name="src"></param>
/// <param name="vp_src"></param>
/// <returns></returns>
bool hitTile(dVector2 p, dVector2 tl, tile const& src, dRect vp_src);

