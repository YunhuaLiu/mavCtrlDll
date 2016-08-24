//==============================================================================
//
// Title:		MavCtrl_CVI.h
// Purpose:		A short description of the interface.
//
// Created on:	4/26/2016 at 4:07:11 PM by Yunhua Liu.
// Copyright:	Flextronics. All Rights Reserved.
//
//==============================================================================

#ifndef __MavCtrl_CVI_H__
#define __MavCtrl_CVI_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "cvidef.h"

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// External variables

//==============================================================================
// Global functions

int mavlink_query(int port,int test_type,int timeout_ms,const char * t_cmd); 

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __MavCtrl_CVI_H__ */
