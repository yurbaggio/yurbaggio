/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		May 4, 2014
Modified	May 4, 2014
Device wrapper for 9 axis IMU
******************************************************************************/

#include <stdlib.h>

#include "nlib_dev_mpu9150.h"
#include "nlib_sys_err.h"
#include "inv_mpu.h"
#include "inv_mpu_orientation.h"
#include "inv_mpu_dmp_motion_driver.h"

//constructor reserve memory
MpuObj mpu_New(){
	MpuObj self = (MpuObj)calloc(1, sizeof(struct mpuObj));
	return self;
}

//initiate imu (memory allocation and linking only)
int mpu_Init(MpuObj self, MpuFunc fTable_var, int mpuRate_var, int magMix_var, int magRate_var, int mpuLpf_var, const signed char *mpuOrient_var)
{
	if (self == NULL)
		return NLIB_ERR_NOMEM;
	
	//link function tables
	self->fTable = fTable_var;
	
	//check for input validity
	if ((magRate_var > 100) || (magRate_var < 1))
		return NLIB_ERR_INVALPARAM;
	if ((mpuRate_var > 1000) || (mpuRate_var < 1))
		return NLIB_ERR_INVALPARAM;
	
	//assign init data
	self->mpuRate = mpuRate_var;
	self->magMix = magMix_var;
	self->magRate = magRate_var;
	self->mpuLpf = mpuLpf_var;
	self->mpuOrient = inv_orientation_matrix_to_scalar(mpuOrient_var);
	
	return NLIB_ERR_NOERROR;
}

//start imu (power is good and ready for initial calibration
int mpu_Start(MpuObj self){
	
	//result of function calls
	int result;
	
	//retrieve calibration data
	
	//no interrupts needed, place dummy interrupt structure for init
	struct int_param_s int_param;
	int_param.cb = NULL;
	int_param.pin = 0;
	int_param.lp_exit = 0;
	int_param.active_low = 1;
	
	//initiate mpu, return with error code if failed
	result = mpu_init(&int_param);
	if (result != 0)
		return result;
	
	//if successful, enable sensors, configure sensors
	mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL | INV_XYZ_COMPASS);   // enable all of the sensors
	mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL);    
	
	//load motion sensor fusions firmware
	result = dmp_load_motion_driver_firmware();
	if (result != 0)
		return result;
	
	//if successful, set up orientation
	dmp_set_orientation(self->mpuOrient); // set up the correct orientation
	dmp_enable_feature(DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_SEND_CAL_GYRO | DMP_FEATURE_GYRO_CAL);
	dmp_set_fifo_rate(self->mpuRate);
	
	//start dmp
	result = mpu_set_dmp_state(1);
	if (result != 0)
		return result;
	
	//set the update rate, set the compass update rate to match
	mpu_set_sample_rate(self->mpuRate);
	mpu_set_compass_sample_rate(self->magRate);
	if (self->mpuLpf != 0)
		mpu_set_lpf(self->mpuLpf);

	//no errors
	return 0;
}

//read from accel device
int mpu_Read(){
	return 0;
}
