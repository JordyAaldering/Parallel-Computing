#ifndef SIMPLE_H_
#define SIMPLE_H_

/**
 * Returns a string in a static buffer that represents the memory
 * amount provided in bytes in a human readable form (GB/MB/KB/byte).
 */
extern char *getMemStr(size_t bytes);

/**
 * Returns a string in a static buffer that represents the time
 * provided ms in a human readable form (min, sec, ms).
 */
extern char *getTimeStr(double ms);

/**
 * Reads file with name "fName" into a freshly allocated string.
 */
extern char *readOpenCL(char *fName);

/**
 OpenCl Functions

     1) These are just wrappers for getting started very easily. The simplest
        way looks roughly like this:

        initCPU()  or  initGPU()  or  initCPUVerbose()  or  initGPUVerbose()

        kernel = setupKernel( ...kernel-string....host-arguments...)

        runKernel( kernel, .... thread space description ....)

        freeDevice()

        NB: the different init versions choose different devices and add
            some debug output to stdout that may help understanding what
            happens :-)

     2) If more control is desired, one may want to replace the use of
        setupKernel and runKernel by lower level wrappers around the openCL
        functions or the openCL functions themselves:
        a) setupKernel consist of:

           kernel = createKernel( ... kernel-string...)
           
           /
             buf = allocDev( ...size...)
             host2dev<arg-type>( ...host-buffer..., buf)
             clSetKernelArg( ....)
           \

        b) runKernel consists of:

           launchKernel( kernel, .... thread space description ....)

           /
             dev2host<arg-type>( bug, ...host-buffer...)
           \

     3) All the wrappers here ensure that errors are being checked AND
        they implicitly perform some wallclock time measurements. This
        enables some simple profiling by using:

        printKernelTime()

        and

        printTransferTimes()

     4) There are some more wrapper to extract info from the device:

        maxWorkItems( dim)
 */

/**
 * Sets up the openCL environment for using a GPU.
 * Note that the system may have more than one GPU in which case
 * the one that has been pre-configured will be chosen.
 * If anything goes wrong in the course, error messages will be
 * printed to stderr and the last error encountered will be returned.
 */
extern cl_int initGPU();

/**
 * triggers some additional information to be print to stdout
 * which documents what your code is trying to do.
 * We strongly recommend using this version during development.
 */
extern cl_int initGPUVerbose();

/**
 *  Sets up the openCL environment for using the host machine.
 *  If anything goes wrong in the course, error messages will be
 *  printed to stderr and the last error encountered will be returned.
 *  Note that this may go wrong as not all openCL implementations
 *  support this!
 */
extern cl_int initCPU ();

/**
 * Triggers some additional information to be print to stdout
 * which documents what your code is trying to do.
 * We strongly recommend using this version during development.
 */
extern cl_int initCPUVerbose ();

typedef enum {
  DoubleArr,
  FloatArr,
  IntArr,
  BoolArr,
  IntConst
} clarg_type;

/**
 * This routine prepares a kernel for execution.
 * If anything goes wrong in the course, error messages will be printed to stderr.
 * The pointer to the fully prepared kernel will be returned.
 *
 * Note that this function actually performs quite a few openCL tasks.
 * It compiles the source, it allocates memory on the device and it copies over all float arrays.
 * If a more sophisticated behaviour is needed you may have to fall back to using openCL directly.
 *
 * @param kernel_source The kernel source as a string
 * @param kernel_name The name of the kernel function as string
 * @param num_args The number of arguments (must match those specified in the kernel source!)
 * @param ... The actual arguments. Each argument to the kernel
 *  results in two or three arguments to this function, depending
 *  on whether these are pointers to float-arrays or integer values:
 *  legal argument sets are:
 *   doubleArr::clarg_type, num_elems::int, pointer::double *,
 *   FloatArr::clarg_type, num_elems::int, pointer::float *,
 *   IntConst::clarg_type, number::int
 */
extern cl_kernel setupKernel( const char *kernel_source, char *kernel_name, int num_args, ...);

/**
 * This routine is similar to launchKernel.
 * However, in addition to launching the kernel, it also copies back
 * *all* arguments set up by the previous call to setupKernel!
 */
extern cl_int runKernel( cl_kernel kernel, int dim, size_t *global, size_t *local);

/**
 * This routine releases all acquired resources.
 * If anything goes wrong in the course, error messages will be
 * printed to stderr and the last error encountered will be returned.
 */
extern cl_int freeDevice();

/**
 * Returns an openCL device memory identifier for device memory of "n" bytes.
 */
extern cl_mem allocDev( size_t n);

/**
 * Transfers "n" elements of <type> of the array "a"
 * on the host to the device buffer at "ad".
 */
extern void host2devDoubleArr(double *a, cl_mem ad, size_t n);
extern void host2devFloatArr(float *a, cl_mem ad, size_t n);
extern void host2devIntArr(int *a, cl_mem ad, size_t n);
extern void host2devBoolArr(bool *a, cl_mem ad, size_t n);

/**
 * Transfers "n" elements of the array "ad" of elements of
 * <type> on the device to the host buffer at "a".
 */
extern void dev2hostDoubleArr( cl_mem ad, double *a, size_t n);
extern void dev2hostFloatArr( cl_mem ad, float *a, size_t n);
extern void dev2hostIntArr( cl_mem ad, int *a, size_t n);
extern void dev2hostBoolArr( cl_mem ad, bool *a, size_t n);

/**
 * This routine creates a kernel from the source as string.
 * @param kernel_source the kernel source as a string
 * @param kernel_name the name of the kernel function as string
 * @return
 */
extern cl_kernel createKernel(const char *kernel_source, char *kernel_name);

/**
 * This routine executes the kernel given as first argument.
 * If anything goes wrong in the course, error messages will be
 * printed to stderr and the last error encountered will be returned.
 * @param kernel The kernel.
 * @param dim Identifies the dimensionality of the thread-space.
 * @param global A vector of length <dim> that gives the upper bounds for all axes.
 * @param local Specifies the size of the individual warps,
 *  which need to have the same dimensionality as the overall range.
 * @return
 */
extern cl_int launchKernel(cl_kernel kernel, int dim, size_t *global, size_t *local);

/**
 * We internally measure the wallclock time that elapses
 * during the kernel execution on the device. This routine
 * prints the findings to stdout.
 * Note that the measurement does not include any data
 * transfer times for arguments or results! Note also, that
 * the only functions that influence the time values are
 * launchKernel and runKernel. It does not matter how much
 * time elapses between the last call to runKernel and the
 * call to printKernelTime!
 */
extern void printKernelTime();
extern void printTransferTimes();

/**
 * Returns the maximum number of work items per work group of the selected
 * device in dimension dim. It requires dim to be in {0, 1, 2}.
 */
extern size_t maxWorkItems (int dim);

#endif /* SIMPLE_H_ */
