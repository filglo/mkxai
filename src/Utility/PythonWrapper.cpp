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

std::vector<PyObject*> PythonWrapper::PyTuple::Unpack( PyObject* tuple )
{
    if( tuple != nullptr && PyTuple_Check( tuple ) )
    {
        std::vector<PyObject*> unpackedTuple;
        for( int i = 0; i < PyTuple_Size( tuple ); ++i )
        {
            unpackedTuple.push_back( PyTuple_GetItem( tuple, i ) );
            Py_INCREF( unpackedTuple.back() );
        }
        return unpackedTuple;
    }
    return std::vector<PyObject*>();
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
                //Py_DECREF( pValue );
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

void PythonWrapper::Eval( int& detections, NpyArrayWrapper& boxes, NpyArrayWrapper& scores, NpyArrayWrapper& classes, const cv::Mat& mat )
{
    PyObject* pValue = Call( "Eval", MatToPyObject( mat ) );
    auto tuple = PyTuple::Unpack( pValue );
    boxes = NpyArrayWrapper( tuple[0] );
    scores = NpyArrayWrapper( tuple[1] );
    classes = NpyArrayWrapper( tuple[2] );
    detections = (int)PyFloat_AsDouble( tuple[3] );
    int errorCode = (int)PyFloat_AsDouble( tuple[4] );
    if( errorCode != 0 )
        throw std::runtime_error( "Python error code: " + std::to_string( errorCode ) + "\n" );
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

NpyArrayWrapper::NpyArrayWrapper()
    : m_data( nullptr )
    , m_dim( -1 )
    , m_shape( nullptr )
{}

NpyArrayWrapper::NpyArrayWrapper( PyObject * object )
    : m_data( nullptr )
    , m_dim( -1 )
    , m_shape( nullptr )
{
    if( object )
    {
        if( PyArray_Check( object ) )
        {
            PyArrayObject* npyArray = reinterpret_cast<PyArrayObject*>(object);
            m_dim = PyArray_NDIM( npyArray );
            m_shape = PyArray_SHAPE( npyArray );
            m_data = static_cast<float*>(PyArray_DATA( npyArray ));
            if( !PyArray_CHKFLAGS( npyArray, NPY_ARRAY_CARRAY_RO ) )
            {
                throw std::runtime_error( "Numpy array does not behave\n" );
            }
        }
        else
        {
            throw std::runtime_error( "PyObject is not Numpy array\n" );
        }
    }
    else
    {
        throw std::runtime_error( "Not an object - error\n" );
    }
}

float NpyArrayWrapper::Get( int idx ) const
{
    if( m_dim != 1 )
        throw std::logic_error( "Invalid array size\n" );
    if( idx < 0 || idx >= m_shape[0] )
        throw std::logic_error( "Invalid array index\n" );
    return m_data[idx];
}

float NpyArrayWrapper::Get( int idx, int idx2 ) const
{
    if( m_dim != 2 )
        throw std::logic_error( "Invalid array size\n" );
    if( idx < 0 || idx2 < 0 || idx >= m_shape[0] || idx2 >= m_shape[1] )
        throw std::logic_error( "Invalid array index\n" );
    return m_data[idx * m_shape[1] + idx2];
}

float NpyArrayWrapper::Get( int idx, int idx2, int idx3 ) const
{
    if( m_dim != 3 )
        throw std::logic_error( "Invalid array size\n" );
    if( idx < 0 || idx2 < 0 || idx3 < 0 || idx >= m_shape[0] || idx2 >= m_shape[1] || idx3 >= m_shape[2] )
        throw std::logic_error( "Invalid array index\n" );
    return m_data[(idx * m_shape[1] + idx2) * m_shape[2] + idx3];
}
