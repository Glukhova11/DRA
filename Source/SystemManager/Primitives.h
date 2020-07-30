/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Primitives.h
 * Author: pashkova
 *
 * Created on April 20, 2018, 3:39 PM
 */

#ifndef __DRA_SYSTEMMANAGEMENT_PRIMITIVES_H__
#define __DRA_SYSTEMMANAGEMENT_PRIMITIVES_H__

#include <TCM/Common/GroupBase.h>
#include <ATE/Concept/Primitive.h>

class Gr_SystemManager
{
public:
	enum { GROUP_ID = GR_DRA_BASE};
	enum {
		DRA_SYS_MGR_INIT_REQ
	};
};

ATE_P_DECLARE_SIMPLE_PRIM_CLASS( Gr_SystemManager, DRA_SYS_MGR_INIT_REQ); 

#endif /* __DRA_SYSTEMMANAGEMENT_PRIMITIVES_H__ */

