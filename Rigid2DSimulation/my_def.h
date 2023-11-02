#pragma once
#define dt (cur.fps.dt())
#define dur (cur.fps.dur())
#define scr (cur.scr)
#define dscr (cur.dscr)
#define ft (cur.ft)
#define loc (cur.loc)

#define msw (cur.input.msw)
#define chs (cur.input.chs)
#define msp (cur.input.msp)
#define msp_old (cur.input.msp_old)
#define msd (cur.input.msd)
#define msd_old (cur.input.msd_old)
#define msnd (!msd[0] && !msd[1] && !msd[2])
#define msc (cur.input.msc)
#define kbmsg (cur.input.kbmsg)
#define kbd (cur.input.kbd)
#define kbd_old (cur.input.kbd_old)
#define kbc (cur.input.kbc)

#define own (cur.own)
#define hvd (own.hvd)
#define hoveredDepth (own.hoveredDepth)
#define whd (own.whd)
#define whdDepth (own.whdDepth)
#define keyboardOwner (own.keyboardOwner)
#define free_kb (own.free_kb)
#define rmv own.remove(this);
#define rhvd (hovered && msnd)

#define mkcl(x) mkp(x)(tmp); tmp = {};

#define ui (*cur.ui)
#define bgr (*cur.bgr)
#define gl (cur.gl)
#define dbstr (cur.dbstr)
