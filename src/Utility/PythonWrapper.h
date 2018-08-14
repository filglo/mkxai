#ifndef PYTHONWRAPPER_H
#define PYTHONWRAPPER_H

#ifdef _DEBUG
#undef _DEBUG
#include <python.h>
#define _DEBUG
#else
#include <python.h>
#endif
#include <opencv2/highgui/highgui.hpp>
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <arrayobject.h>
#include <vector>

// Python 3.5 C API & numpy wrappers and helpers

class NpyArrayWrapper
{
public:
    NpyArrayWrapper();
    NpyArrayWrapper( PyObject* object );
    NpyArrayWrapper( const NpyArrayWrapper& other ) { *this = other; }
    NpyArrayWrapper( NpyArrayWrapper&& other ) { *this = other; }
    NpyArrayWrapper& operator=( const NpyArrayWrapper& other )
    {
        // Memory is already allocated, no need to deallocate
        m_data = other.m_data;
        m_dim = other.m_dim;
        m_shape = other.m_shape;
        return *this;
    }
    NpyArrayWrapper& operator=( NpyArrayWrapper&& other )
    {
        m_data = other.m_data;
        m_dim = other.m_dim;
        m_shape = other.m_shape;
        // move assignment - how to use
        //*this = other;
        //throw std::logic_error( "Code mistake\n" );
        return *this;
    }
    float Get( int idx ) const;
    float Get( int idx, int idx2 ) const;
    float Get( int idx, int idx2, int idx3 ) const;
    int GetShape( int idx ) { return static_cast<int>(m_shape[idx]); }
    int GetDims() { return m_dim; }
    bool IsNull() { return m_data == nullptr; }
private:
    float* m_data;
    int m_dim;
    npy_intp* m_shape;
};

class PythonWrapper
{
public:
    PythonWrapper();
    ~PythonWrapper();
    void Eval( int& detections, NpyArrayWrapper& boxes, NpyArrayWrapper& scores, NpyArrayWrapper& classes, const cv::Mat& mat );
private:
    class PyTuple
    {
    public:
        PyTuple();
        template<typename... Args>PyTuple( Args... args );
        ~PyTuple();
        operator PyObject*() const
        {
            return m_tuple;
        }
        static std::vector<PyObject*> Unpack( PyObject* tuple );
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
