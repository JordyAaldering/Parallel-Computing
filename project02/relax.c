#include <stdio.h>
#include <stdlib.h>
#include "CL/cl.h"

static cl_platform_id cpPlatform;   /* openCL platform. */
static cl_device_id device_id;      /* Compute device id. */
static cl_context context;          /* Compute context. */
static cl_command_queue commands;   /* Compute command queue. */
static cl_program program;          /* Compute program. */

cl_int initDevice(int devType) {
    cl_int err = CL_SUCCESS;
    cl_uint num_platforms;
    cl_platform_id *cpPlatforms;
    cl_uint num_devices;
    cl_device_id *cpDevices = NULL;

    /* Connect to a compute device.  */
    err = clGetPlatformIDs(0, NULL, &num_platforms);
    if (CL_SUCCESS != err) {
        exit(1);
    } else {
        cpPlatforms = (cl_platform_id*)malloc(sizeof(cl_platform_id) * num_platforms);
        err = clGetPlatformIDs(num_platforms, cpPlatforms, NULL);

        for (uint8_t i = 0; i < num_platforms; i++) {
            err = clGetDeviceIDs(cpPlatforms[i], devType, 1, &device_id, &num_devices);
            if (err == CL_SUCCESS) {
                cpPlatform = cpPlatforms[i];
                break;
            }
        }

        if (err != CL_SUCCESS) {
            exit(1);
        } else {
            context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
            if (!context || err != CL_SUCCESS) {
                exit(1);
            } else {
                commands = clCreateCommandQueue(context, device_id, 0, &err);
                if (!commands || err != CL_SUCCESS) {
                    exit(1);
                }
            }
        }
    }

    return err;
}

int main() {
    cl_kernel kernel;
    size_t global[1];
    size_t local[1];

    initDevice(CL_DEVICE_TYPE_CPU);

    return 0;
}
