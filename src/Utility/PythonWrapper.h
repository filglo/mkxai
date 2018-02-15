#ifndef PYTHONWRAPPER_H
#define PYTHONWRAPPER_H

#include <Python.h>
#include <opencv2/highgui/highgui.hpp>
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <arrayobject.h>
#include <vector>

// Python 3.5 C API Wrapper
class PythonWrapper
{
public:
    PythonWrapper();
    ~PythonWrapper();
    void Eval( int& out, const cv::Mat& mat );
private:
    class PyTuple
    {
    public:
        PyTuple();
        template<typename... Args>PyTuple( Args... args );
        ~PyTuple();
        operator PyObject* () const
        {
            return m_tuple;
        }
    private:
        template<typename... Args> void Pack( PyObject* element, Args... args );
        void Pack( PyObject* element );

        PyObject* m_tuple;
        int m_index;
    };
    template<typename... Args> PyObject* Call( const std::string& name, Args... args ) const;
    PyObject* MatToPyObject( const cv::Mat& mat ) const;

    int ImportModules();
    PyObject* m_pModule;
};

#endif // !PYTHONWRAPPER_H
