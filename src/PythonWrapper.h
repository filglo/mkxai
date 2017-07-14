#ifndef PYTHONWRAPPER_H
#define PYTHONWRAPPER_H

#include <Python.h>
#include <opencv2/highgui/highgui.hpp>
#include <D:\Anaconda\Lib\site-packages\numpy\core\include\numpy\arrayobject.h>

// Python 3.5 C API Wrapper
class PythonWrapper
{
public:
    PythonWrapper();
    ~PythonWrapper();
    PyObject* Call( const std::string& name, PyObject* arg );
    int CallEval( const cv::Mat& mat );
private:
    int ImportModules();
    PyObject* m_pModule;
};

#endif // !PYTHONWRAPPER_H
