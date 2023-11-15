#pragma once
#define dt (cur.fps.dt())
#define dur (cur.fps.dur())
#define scr (cur.scr)
#define dscr (cur.dscr)
#define ft (cur.ft)
#define getLocalizedString (cur.getLocalizedString)

#define msw (cur.input.msw)
#define chs (cur.input.chs)
#define msp (cur.input.msp)
#define msp_old (cur.input.msp_old)
#define msd (cur.input.msd)
#define msd_old (cur.input.msd_old)
#define msnd (!msd[0] && !msd[1] && !msd[2])
#define msc (cur.input.isMouseClickedDown)
#define kbmsg (cur.input.kbmsg)
#define kbd (cur.input.kbd)
#define kbd_old (cur.input.kbd_old)
#define kbc (cur.input.isKeyPressedDown)

#define own (cur.own)
#define hvd (own.hovered)
#define hoveredDepth (own.hoveredDepth)
#define whd (own.wheeled)
#define whdDepth (own.wheeledDepth)
#define keyboardOwner (own.keyboardOwner)
#define free_kb (own.free_kb)
#define rmv own.remove(this);
#define rhvd (hovered && msnd)

#define mkcl(x) mkp(x)(tmp); tmp = {};

#define ui (*cur.ui)
#define bgr (*cur.mBgr)
#define gl (cur.curScope)
#define dbstr (cur.dbstr)
