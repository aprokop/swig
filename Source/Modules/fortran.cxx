#include "swigmod.h"
#include "cparse.h"

class FORTRAN : public Language
{
  private:
    // >>> ATTRIBUTES

    String *d_module; //!< Module name
    String *d_outpath; //!< WRAP.cxx output

    // >>> OUTPUT FILES

    // Injected into .cxx file
    String *f_begin; //!< Very beginning of output file
    String *f_runtime; //!< SWIG runtime code
    String *f_header; //!< Declarations and inclusions from .i
    String *f_wrappers; //!< C++ Wrapper code
    String *f_init; //!< C++ initalization functions

    // Injected into _I file
    String *f_wrapper_interfaces; //!< interface code inside the _I file
    // Injected into _M file
    String *f_imports;     //!< Fortran "use" directives generated from %import
    String *f_types;       //!< Generated class types
    String *f_interfaces;  //!< Fortran _M: generated classes
    String *f_subroutines; //!< Fortran _M: generated classes

    // Side effect attributes
    bool d_in_constructor; //!< Whether we're being called inside a constructor
    bool d_in_destructor; //!< Whether we're being called inside a constructor

  public:
    virtual void main(int argc, char *argv[]);
    virtual int top(Node *n);
    virtual int functionWrapper(Node *n);
    virtual int destructorHandler(Node *n);
    virtual int constructorHandler(Node *n);
    virtual int classHandler(Node *n);
    virtual int memberfunctionHandler(Node *n);

    FORTRAN()
    {
        /* * */
    }

  private:
    void write_wrapper();
    void write_module_interface();
    void write_module_code();
};

//---------------------------------------------------------------------------//
/*!
 * \brief Main function for code generation.
 *
 * TODO: add flag for writing automatic "final" destructors in fortran.
 */
void FORTRAN::main(int argc, char *argv[])
{
    (void)argc; (void)argv;

    /* Set language-specific subdirectory in SWIG library */
    SWIG_library_directory("fortran");

    /* Set language-specific preprocessing symbol */
    Preprocessor_define("SWIGFORTRAN 1", 0);

    /* Set typemap language (historical) */
    SWIG_typemap_lang("fortran");

    /* Set language-specific configuration file */
    SWIG_config_file("fortran.swg");
}

//---------------------------------------------------------------------------//
/*!
 * \brief Top-level code generation function.
 */
int FORTRAN::top(Node *n)
{
    // Module name (from the SWIG %module command)
    d_module = Getattr(n, "name");
    // Output file name
    d_outpath = Getattr(n, "outfile");

    /* Initialize temporary file-like output strings */

    // run time code (beginning of .cxx file)
    f_begin = NewString("");
    Swig_register_filebyname("begin", f_begin);

    // run time code (beginning of .cxx file)
    f_runtime = NewString("");
    Swig_register_filebyname("runtime", f_runtime);

    // header code (after run time)
    f_header = NewString("");
    Swig_register_filebyname("header", f_header);

    // C++ wrapper code (middle of .cxx file)
    f_wrappers = NewString("");
    Swig_register_filebyname("wrapper", f_wrappers);

    // initialization code (end of .cxx file)
    f_init = NewString("");
    Swig_register_filebyname("init", f_init);

    // Fortran interfaces (injected into _I.f90)
    f_wrapper_interfaces = NewString("");
    Swig_register_filebyname("fwrapperinterfaces", f_wrapper_interfaces);

    // Other imported fortran modules
    f_imports = NewString("");
    Swig_register_filebyname("fimports", f_imports);

    // Fortran classes
    f_types = NewString("");
    Swig_register_filebyname("ftypes", f_types);

    // Fortran class constructors
    f_interfaces = NewString("");
    Swig_register_filebyname("finterfaces", f_interfaces);

    // Fortran subroutines
    f_subroutines = NewString("");
    Swig_register_filebyname("fsubroutines", f_subroutines);

    /* Emit all other wrapper code */
    Language::top(n);

    /* Write fortran module files */
    write_wrapper();
    write_module_interface();
    write_module_code();

    /* Cleanup files */
    Delete(f_runtime);
    Delete(f_header);
    Delete(f_wrappers);
    Delete(f_init);
    Delete(f_begin);

    return SWIG_OK;
}

//---------------------------------------------------------------------------//
/*!
 * \brief Write C++ wrapper code
 */
void FORTRAN::write_wrapper()
{
    // Open file
    File* out = NewFile(d_outpath, "w", SWIG_output_files());
    if (!out)
    {
        FileErrorDisplay(d_outpath);
        SWIG_exit(EXIT_FAILURE);
    }

    // Write SWIG auto-generation banner
    Swig_banner(out);

    // Write three different levels of output
    Dump(f_begin,    out);
    Dump(f_runtime,  out);
    Dump(f_header,   out);

    // Write wrapper code
    Printf(out, "#ifdef __cplusplus\n");
    Printf(out, "extern \"C\" {\n");
    Printf(out, "#endif\n");
    Dump(f_wrappers, out);
    Printf(out, "#ifdef __cplusplus\n");
    Printf(out, "}\n");
    Printf(out, "#endif\n");

    // Write initialization code
    Wrapper_pretty_print(f_init, out);

    // Close file
    Delete(out);
}

//---------------------------------------------------------------------------//
/*!
 * \brief Write Fortran interface module
 */
void FORTRAN::write_module_interface()
{
    // Open file
    String* path = NewStringf(
            "%s%s_I.f90", SWIG_output_directory(), Char(d_module));
    File* out = NewFile(path, "w", SWIG_output_files());
    if (!out)
    {
        FileErrorDisplay(path);
        SWIG_exit(EXIT_FAILURE);
    }
    Delete(path);

    // Write SWIG auto-generation banner
    Swig_banner_target_lang(out, "!");

    // Write fortran header
    Printf(out, "module %s_I\n", d_module);
    Printf(out, " use, intrinsic :: ISO_C_BINDING\n"
                " implicit none\n"
                " interface\n");

    Printv(out, f_wrapper_interfaces, NULL);

    Printf(out, " end interface\n"
                "end module %s_I\n", d_module);

    // Close file
    Delete(out);
}

//---------------------------------------------------------------------------//
/*!
 * \brief Write Fortran implementation module
 */
void FORTRAN::write_module_code()
{
    // Open file
    String* path = NewStringf(
            "%s%s_M.f90", SWIG_output_directory(), Char(d_module));
    File* out = NewFile(path, "w", SWIG_output_files());
    if (!out)
    {
        FileErrorDisplay(path);
        SWIG_exit(EXIT_FAILURE);
    }
    Delete(path);

    // Write SWIG auto-generation banner
    Swig_banner_target_lang(out, "!");

    // Write fortran header
    Printf(out, "module %s_M\n", d_module);
    Printf(out, " use, intrinsic :: ISO_C_BINDING\n");
    Printf(out, " use %s_I\n", d_module);
    Printv(out, f_imports, NULL);
    Printf(out, " implicit none\n");
    Printf(out, " ! TYPES\n");
    Printv(out, f_types, NULL);
    Printf(out, " ! INTERFACES\n");
    Printv(out, f_interfaces, NULL);
    Printf(out, "contains\n");
    Printv(out, f_subroutines, NULL);
    Printf(out, "end module %s_M\n", d_module);

    // Close file
    Delete(out);
}

//---------------------------------------------------------------------------//
/*!
 * \brief Wrap basic functions.
 *
 * This is also passed class methods.
 */
int FORTRAN::functionWrapper(Node *n)
{
    //bool is_destructor = (Cmp(Getattr(n, "nodeType"), "destructor") == 0);

    // Basic attributes (references)
    String* symname = Getattr(n, "sym:name");
    String* nodetype = Getattr(n, "nodeType");
    SwigType* type = Getattr(n, "type");
    ParmList* parmlist = Getattr(n, "parms");
    String* orig_wrapper_name = Swig_name_wrapper(symname);

    // Update wrapper name for overloaded functions
    String* wname = Copy(orig_wrapper_name);
    if (Getattr(n, "sym:overloaded"))
    {
        String* overname = Getattr(n, "sym:overname");
        Append(wname, overname);
    }
    else
    {
        if (!addSymbol(symname, n))
            return SWIG_ERROR;
    }
    Setattr(n, "wrap:name", wname);

    // Add fortran wrapper name
    String* fwname = Copy(wname);
    Replace(fwname, "_wrap_", "", DOH_REPLACE_FIRST);
    Setattr(n, "fortran:name", fwname);

    // A new wrapper function object
    Wrapper* f = NewWrapper();

    // Make a wrapper name for this function

    /* Attach the non-standard typemaps to the parameter list. */
    Swig_typemap_attach_parms("fctype", parmlist, f);
    Swig_typemap_attach_parms("fftype", parmlist, f);

    /* Get return types */

    // Get C type
    String *tm = Swig_typemap_lookup("fctype", n, "", 0);
    String *c_return_type = NewString("");
    if (tm)
    {
        Printf(c_return_type, "%s", tm);
    }
    else
    {
        Swig_warning(WARN_JAVA_TYPEMAP_JNI_UNDEF,
                input_file, line_number, "No fctype typemap defined for %s\n", SwigType_str(type, 0));
    }
    Printv(f->def, "SWIGEXPORT ", c_return_type, " ", wname, "(", NULL);

    // Function attributes
    bool is_subroutine = (Cmp(c_return_type, "void") == 0);
    // Add local variable for result
    if (!is_subroutine)
    {
        Wrapper_add_localv(f, "fresult", c_return_type, "fresult = 0", NULL);
    }

    // SWIG: Emit all of the local variables for holding arguments.
    emit_parameter_variables(parmlist, f);
    emit_attach_parmmaps(parmlist, f);
    Setattr(n, "wrap:parms", parmlist);

    // Wrappers not wanted for some methods where the parameters cannot be overloaded in Fortran
    if (Getattr(n, "sym:overloaded"))
    {
        // Emit warnings for the few cases that can'type be overloaded in Fortran and give up on generating wrapper
        Swig_overload_check(n);
        if (Getattr(n, "overload:ignore"))
        {
            DelWrapper(f);
            return SWIG_OK;
        }
    }

    // Now walk the function parameter list and generate code to get arguments
    Parm* p = parmlist;
    String* nondir_args = NewString("");

    // Parameter output inside the subroutine/function
    String* fortran_parameters = NewString("");
    Printf(fortran_parameters, "    use, intrinsic :: ISO_C_BINDING\n");
    Printf(fortran_parameters, "    implicit none\n");

    if (!is_subroutine)
    {
        // Get corresponding Fortran type
        tm = Swig_typemap_lookup("fftype", n, "", 0);
        String *f_return_type = NewString("");
        if (tm)
        {
            Printf(f_return_type, "%s", tm);
        }
        else
        {
            Swig_warning(WARN_JAVA_TYPEMAP_JTYPE_UNDEF,
                    input_file, line_number,
                    "No fftype typemap defined for %s\n", SwigType_str(type, 0));
        }

        Printf(fortran_parameters, "    %s :: fresult\n", f_return_type);
        Delete(f_return_type);
    }

    String* fortran_arglist = NewString("(");

    int num_arguments = emit_num_arguments(parmlist);
    for (int i = 0; i < num_arguments; ++i)
    {
        while (checkAttribute(p, "tmap:in:numinputs", "0"))
        {
            p = Getattr(p, "tmap:in:next");
        }

        const char* prepend_comma = (i == 0 ? "" : ", ");

        // Construct conversion argument name
        String *arg = NewString("");
        String *lname = Getattr(p, "lname");
        Printf(arg, "f%s", lname);

        /* Get the C types of the parameter */
        String *c_param_type = NewString("");
        tm = Getattr(p, "tmap:fctype");
        if (tm)
        {
            Printv(c_param_type, tm, NULL);
        }
        else
        {
            SwigType *param_type = Getattr(p, "type");
            Swig_warning(WARN_JAVA_TYPEMAP_JNI_UNDEF, input_file, line_number,
                    "No fctype typemap defined for %s\n", SwigType_str(param_type, 0));
        }
        Printv(f->def, prepend_comma, c_param_type, " ", arg, NULL);

        /* Add parameter to intermediary class declarations */
        String *f_param_type = NewString("");
        tm = Getattr(p, "tmap:fftype");
        if (tm)
        {
            Printv(f_param_type, tm, NULL);
        }
        else
        {
            SwigType *param_type = Getattr(p, "type");
            Swig_warning(WARN_JAVA_TYPEMAP_JTYPE_UNDEF, input_file, line_number,
                    "No fftype typemap defined for %s\n", SwigType_str(param_type, 0));
        }
        // Add parameter name to declaration list
        Printv(fortran_arglist, prepend_comma, arg, NULL);
        // Add parameter type to the parameters list
        Printv(fortran_parameters, "    ", f_param_type, " :: ", arg, "\n", NULL);

        /* Add parameter to C function */

        // Get typemap for this argument
        if ((tm = Getattr(p, "tmap:in")))
        {
            Replaceall(tm, "$input", arg);
            Setattr(p, "emit:input", arg);

            Printf(nondir_args, "%s\n", tm);

            p = Getattr(p, "tmap:in:next");
        }
        else
        {
            SwigType *param_type = Getattr(p, "type");
            Swig_warning(WARN_TYPEMAP_IN_UNDEF, input_file, line_number,
                         "Unable to use type %s as a function argument.\n", SwigType_str(param_type, 0));
            p = nextSibling(p);
        }

        Delete(f_param_type);
        Delete(c_param_type);
        Delete(arg);
    }
    Printf(fortran_arglist, ")");

    if (!is_subroutine)
    {
        Printf(fortran_arglist, " &\n      result(fresult)");
    }

    Printv(f->code, nondir_args, NULL);
    Delete(nondir_args);

    /* Insert constraint checking code */
    p = parmlist;
    while (p)
    {
        if ((tm = Getattr(p, "tmap:check")))
        {
            Replaceall(tm, "$input", Getattr(p, "emit:input"));
            Printv(f->code, tm, "\n", NULL);
            p = Getattr(p, "tmap:check:next");
        }
        else
        {
            p = nextSibling(p);
        }
    }

    /* Insert cleanup code */
    String *cleanup = NewString("");
    p = parmlist;
    while (p)
    {
        if ((tm = Getattr(p, "tmap:freearg")))
        {
            Replaceall(tm, "$input", Getattr(p, "emit:input"));
            Printv(cleanup, tm, "\n", NULL);
            p = Getattr(p, "tmap:freearg:next");
        }
        else
        {
            p = nextSibling(p);
        }
    }

    /* Insert argument output code */
    String *outarg = NewString("");
    p = parmlist;
    while (p)
    {
        if ((tm = Getattr(p, "tmap:argout"))) {
            Replaceall(tm, "$result", "fresult");
            Replaceall(tm, "$input", Getattr(p, "emit:input"));
            Printv(outarg, tm, "\n", NULL);
            p = Getattr(p, "tmap:argout:next");
        }
        else
        {
            p = nextSibling(p);
        }
    }

    // Now write code to make the function call
    Swig_director_emit_dynamic_cast(n, f);
    String *actioncode = emit_action(n);

    /* Return value if necessary  */
    tm = Swig_typemap_lookup_out("out", n, Swig_cresult_name(), f, actioncode);
    if (tm)
    {
        Replaceall(tm, "$result", "fresult");
        Replaceall(tm, "$owner", (GetFlag(n, "feature:new") ? "1" : "0"));

        Printf(f->code, "%s", tm);
        if (Len(tm))
            Printf(f->code, "\n");
    }
    else
    {
        Swig_warning(WARN_TYPEMAP_OUT_UNDEF, input_file, line_number,
                "Unable to use return type %s in function %s.\n",
                SwigType_str(type, 0), Getattr(n, "name"));
    }
    emit_return_variable(n, type, f);

    /* Output argument output code */
    Printv(f->code, outarg, NULL);

    /* Output cleanup code */
    Printv(f->code, cleanup, NULL);

    /* Finish C function and intermediary class function definitions */
    Printf(f->def, ") {");

    if (!is_subroutine)
        Printv(f->code, "    return fresult;\n", NULL);
    Printf(f->code, "}\n");

    /* Standard SWIG substitutions */
    Replaceall(f->code, "$cleanup", cleanup);
    Replaceall(f->code, "$symname", symname);
    Replaceall(f->code, "SWIG_contract_assert(", "SWIG_contract_assert($null, ");
    Replaceall(f->code, "$null", (is_subroutine ? "" : "0"));

    /* Write the C++ function into f_wrappers */
    Wrapper_print(f, f_wrappers);

    /* Write the Fortran interface file */
    const char* sub_or_func = (is_subroutine ? "subroutine" : "function");

    Printv(f_wrapper_interfaces,
           "   ", sub_or_func, " ", wname, fortran_arglist,
           " &\n      bind(C, name=\"", wname, "\")\n",
           fortran_parameters,
           "   end ", sub_or_func, "\n",
           NULL);
    Printv(f_subroutines,
           "   ", sub_or_func, " ", fwname, fortran_arglist,
           "\n",
           fortran_parameters,
           "   end ", sub_or_func, "\n",
           NULL);


    Delete(c_return_type);
    Delete(cleanup);
    Delete(outarg);
    Delete(fortran_parameters);
    DelWrapper(f);
    Delete(wname);
    return SWIG_OK;
}

//---------------------------------------------------------------------------//
/*!
 * \brief Process classes.
 */
int FORTRAN::classHandler(Node *n)
{
    // Basic attributes
    String *symname = Getattr(n, "sym:name");
    String *real_classname = Getattr(n, "name");
    SwigType *type = Getattr(n, "type");
    ParmList *parmlist = Getattr(n, "parms");

    if (!addSymbol(symname, n))
        return SWIG_ERROR;

    /* No base classes are currently supported */
    List *baselist = Getattr(n, "bases");
    if (baselist && Len(baselist))
    {
        Swig_warning(WARN_LANG_NATIVE_UNIMPL, Getfile(n), Getline(n),
                "Inheritance (class '%s') currently unimplemented.\n",
                SwigType_namestr(Char(symname)));
    }

    /* Create bare-bones proxy class */
    String* fortran_type = NewString("");
    String* fortran_arglist = NewString("");
    String* fortran_end_declaration = NewString("");

    //Printf(fortran_arglist, );
    //Printf(fortran_end_declaration, " end type\n");

    Printf(fortran_arglist, "  type(C_PTR) :: instance_ptr=C_NULL_PTR\n");
    Printf(fortran_arglist, " contains\n");

    String* fortran_parameters = NewString("");
    String* fortran_subroutines = NewString("");

    /* Emit class members */
    Language::classHandler(n);

    /* Add types */
    String *smartptr = Getattr(n, "feature:smartptr");
    SwigType *smart = 0;
    if (smartptr)
    {
        SwigType *cpt = Swig_cparse_type(smartptr);
        if (cpt)
        {
            smart = SwigType_typedef_resolve_all(cpt);
            Delete(cpt);
        }
        else
        {
            // TODO: report line number of where the feature comes from
            Swig_error(Getfile(n), Getline(n), "Invalid type (%s) in 'smartptr' "
                     " feature for class %s.\n", smartptr, real_classname);
        }
    }
    SwigType *ct = Copy(smart ? smart : real_classname);
    SwigType_add_pointer(ct);
    SwigType *realct = Copy(real_classname);
    SwigType_add_pointer(realct);
    Delete(smart);
    Delete(ct);
    Delete(realct);

    /* Write fortran class header */
    Printf(f_types, " type %s\n", Char(symname));
    Printf(f_types, "  type(C_PTR), private :: ptr = C_NULL_PTR\n");
    Printf(f_types, " contains\n");
    //Printv(f_types, f_class_method_names, NULL);
    Printf(f_types, " end type\n");
    //           fortran_arglist,
    //           fortran_parameters,
    //           fortran_subroutines,
    //           NULL);

    Delete(fortran_subroutines);
    Delete(fortran_parameters);
    Delete(fortran_arglist);

    return SWIG_OK;
}

//---------------------------------------------------------------------------//
/*!
 * \brief Extra stuff for constructors.
 */
int FORTRAN::constructorHandler(Node* n)
{
    Language::constructorHandler(n);
    return SWIG_OK;
}

//---------------------------------------------------------------------------//
/*!
 * \brief Handle extra destructor stuff.
 *
 * TODO: this is where the optional "final" support for Fortran would be added?
 */
int FORTRAN::destructorHandler(Node* n)
{
    Language::destructorHandler(n);
    return SWIG_OK;
}

//---------------------------------------------------------------------------//
/*!
 * \brief Process member functions.
 */
int FORTRAN::memberfunctionHandler(Node *n)
{
    Language::memberfunctionHandler(n);
}

//---------------------------------------------------------------------------//
// Expose the code to the SWIG main function.
//---------------------------------------------------------------------------//
extern "C" Language *
swig_fortran(void)
{
    return new FORTRAN();
}