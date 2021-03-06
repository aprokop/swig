
// Auto-generated by doxy2swig. Do not edit.

// File: index.xml

// File: structBasicStruct.xml
%feature("docstring") BasicStruct "

Simple public struct.

C++ includes: SimpleClass.hh ";


// File: classSimpleClass.xml
%feature("docstring") SimpleClass "

Simple test class.

C++ includes: SimpleClass.hh ";

%feature("docstring")  SimpleClass::SimpleClass "SimpleClass::SimpleClass()

Constructor. ";

%feature("docstring")  SimpleClass::SimpleClass "SimpleClass::SimpleClass(const SimpleClass &rhs)

Copy constructor. ";

%feature("docstring")  SimpleClass::SimpleClass "SimpleClass::SimpleClass(double d)

Other constructor. ";

%feature("docstring")  SimpleClass::~SimpleClass "SimpleClass::~SimpleClass()

Destructor. ";

%feature("docstring")  SimpleClass::set "void
SimpleClass::set(storage_type val)

Set the value. ";

%feature("docstring")  SimpleClass::double_it "void
SimpleClass::double_it()

Multiply the value by 2. ";

%feature("docstring")  SimpleClass::get "storage_type
SimpleClass::get() const

Access the value. ";

%feature("docstring")  SimpleClass::action "void
SimpleClass::action(T &val)

templated function ";

%feature("docstring")  SimpleClass::get_multiplied "storage_type
SimpleClass::get_multiplied(multiple_type multiple) const

Access the value, multiplied by some parameter. ";


// File: SimpleClass_8hh.xml
%feature("docstring")  print_value "void print_value(const
SimpleClass &c)

Free function. ";

%feature("docstring")  make_class "SimpleClass
make_class(SimpleClass::storage_type val)

Return by value should be converted to set-by-reference. ";

%feature("docstring")  get_class "const SimpleClass& get_class()

Return by reference. ";

%feature("docstring")  set_class_by_copy "void
set_class_by_copy(SimpleClass c)

Pass class as a parameter. ";

%feature("docstring")  print_color "void print_color(MyEnum color)

Get a color name. ";

