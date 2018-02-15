#include "PythonWrapper.h"

PythonWrapper::PythonWrapper()
{
    PyObject *pName;

    // Initialize once
    Py_Initialize();
    pName = PyUnicode_DecodeFSDefault( "tfDetection" );
    /* Error checking of pName left out */

    m_pModule = PyImport_Import( pName );
    Py_DECREF( pName );
    ImportModules();
}

PythonWrapper::~PythonWrapper()
{
    Call( "Exit" );
    if( m_pModule )
        Py_DECREF( m_pModule );
    Py_Finalize();
}

PythonWrapper::PyTuple::PyTuple()
    : m_tuple( PyTuple_New( 0 ) )
    , m_index( 0 )
{}

template<typename... Args> PythonWrapper::PyTuple::PyTuple( Args... args )
    : m_tuple( PyTuple_New( sizeof...(Args) ) )
    , m_index( 0 )
{
    Pack( args... );
}

PythonWrapper::PyTuple::~PyTuple()
{
    if( m_tuple != nullptr )
    {
        Py_DECREF( m_tuple );
    }
}

void PythonWrapper::PyTuple::Pack( PyObject* element )
{
    PyTuple_SetItem( m_tuple, m_index, element );
    m_index++;
}

template<typename... Args> void PythonWrapper::PyTuple::Pack( PyObject* element, Args... args )
{
    PyTuple_SetItem( m_tuple, m_index, element );
    m_index++;
    Pack( args... );
}

template<typename... Args> PyObject* PythonWrapper::Call( const std::string& name, Args... args ) const
{
    PyObject *pFunc, *pValue;
    if( m_pModule != NULL )
    {
        pFunc = PyObject_GetAttrString( m_pModule, name.c_str() );
        if( pFunc && PyCallable_Check( pFunc ) )
        {
            pValue = PyObject_CallObject( pFunc, PyTuple( args... ) );
            if( pValue != NULL )
            {
                Py_DECREF( pValue );
            }
            else
            {
                Py_DECREF( pFunc );
                PyErr_Print();
                throw std::runtime_error( "Call failed (NULL return value): " + name + "\n" );
                return nullptr;
            }
            Py_XDECREF( pFunc );
        }
        else
        {
            if( PyErr_Occurred() )
                PyErr_Print();
            Py_XDECREF( pFunc );
            throw std::runtime_error( "Call failed (function not callable): " + name + "\n" );
        }
        return pValue;
    }
    else
    {
        PyErr_Print();
        throw std::runtime_error( "Python module not loaded\n" );
        return nullptr;
    }
}

//PyObject* PythonWrapper::PackTuple( const std::vector<PyObject*> args = std::vector<PyObject*>() ) const
//{
//    PyObject* pArgs = PyTuple_New( args.size() );
//    for( int i = 0; i < args.size(); ++i )
//    {
//        PyTuple_SetItem( pArgs, i, args[i] );
//    }
//    return pArgs;
//}

//
//PyObject* PythonWrapper::Call( const std::string& name, std::vector<PyObject*> args = std::vector<PyObject*>() ) const
//{
//    PyObject *pFunc, *pValue, *pArgs;
//    if( m_pModule != NULL )
//    {
//        pFunc = PyObject_GetAttrString( m_pModule, name.c_str() );
//        if( pFunc && PyCallable_Check( pFunc ) )
//        {
//            pArgs = PackTuple( args );
//            pValue = PyObject_CallObject( pFunc, pArgs );
//            Py_DECREF( pArgs );
//            if( pValue != NULL )
//            {
//                Py_DECREF( pValue );
//            }
//            else
//            {
//                Py_DECREF( pFunc );
//                PyErr_Print();
//                throw std::runtime_error( "Call failed (NULL return value): " + name + "\n" );
//                return nullptr;
//            }
//            Py_XDECREF( pFunc );
//        }
//        else
//        {
//            if( PyErr_Occurred() )
//                PyErr_Print();
//            Py_XDECREF( pFunc );
//            throw std::runtime_error( "Call failed (function not callable): " + name + "\n" );
//        }
//        return pValue;
//    }
//    else
//    {
//        PyErr_Print();
//        throw std::runtime_error( "Python module not loaded\n" );
//        return nullptr;
//    }
//}

void PythonWrapper::Eval( int& out, const cv::Mat& mat )
{
    PyObject* pValue = Call( "Eval", MatToPyObject( mat ) );
    out = PyLong_AsLong( pValue );
}

PyObject* PythonWrapper::MatToPyObject( const cv::Mat& mat ) const
{
    npy_intp dims[3] = { mat.rows, mat.cols, mat.channels() };
    PyObject* numpyArray = PyArray_SimpleNewFromData( 3, dims, NPY_UINT8, (uint8_t*)mat.data );
    return numpyArray;
}

int PythonWrapper::ImportModules()
{
    if( PyArray_API == nullptr )
    {
        import_array();
    }
    return 0;
}
