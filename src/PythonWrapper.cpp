#include "PythonWrapper.h"

PythonWrapper::PythonWrapper()
{
    PyObject *pName;

    // Initialize once
    Py_Initialize();
    pName = PyUnicode_DecodeFSDefault( "test5" );
    /* Error checking of pName left out */

    m_pModule = PyImport_Import( pName );
    Py_DECREF( pName );
    ImportModules();
}

PythonWrapper::~PythonWrapper()
{
    Py_DECREF( m_pModule );
    Py_Finalize();
}

PyObject* PythonWrapper::Call( const std::string& name, PyObject* arg )
{
    PyObject *pFunc, *pValue, *pArgs;
    if( m_pModule != NULL )
    {
        pFunc = PyObject_GetAttrString( m_pModule, name.c_str() );
        if( pFunc && PyCallable_Check( pFunc ) )
        {
            pArgs = PyTuple_New( 1 );
            PyTuple_SetItem( pArgs, 0, arg );
            pValue = PyObject_CallObject( pFunc, pArgs );
            Py_DECREF( pArgs );
            if( pValue != NULL )
            {
                // does it delete?
                Py_DECREF( pValue );
            }
            else
            {
                Py_DECREF( pFunc );
                PyErr_Print();
                throw std::runtime_error( "Call failed\n" );
                return nullptr;
            }
            Py_XDECREF( pFunc );
        }
        else
        {
            if( PyErr_Occurred() )
                PyErr_Print();
            Py_XDECREF( pFunc );
            throw std::runtime_error( "Call failed\n" );
        }
        return pValue;
    }
    else
    {
        PyErr_Print();
        throw std::runtime_error( "Module failed\n" );
        return nullptr;
    }
}

int PythonWrapper::CallEval( const cv::Mat& mat )
{
    npy_intp dims[2] = { mat.rows, mat.cols };
    PyObject* numpyArray = PyArray_SimpleNewFromData( 2, dims, NPY_UINT8, (uint8_t*)mat.data );
    PyObject* pValue = Call( "Eval", numpyArray );
    return PyLong_AsLong( pValue );
}

int PythonWrapper::ImportModules()
{
    if( PyArray_API == nullptr )
    {
        import_array();
    }
    return 0;
}
