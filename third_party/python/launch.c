/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=4 sts=4 sw=4 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/x/x.h"
#include "third_party/python/Include/abstract.h"
#include "third_party/python/Include/import.h"
#include "third_party/python/Include/modsupport.h"
#include "third_party/python/Include/object.h"
#include "third_party/python/Include/pydebug.h"
#include "third_party/python/Include/pylifecycle.h"
#include "third_party/python/Include/pymem.h"
#include "third_party/python/Include/pythonrun.h"
#include "third_party/python/Include/sysmodule.h"
#include "third_party/python/Include/unicodeobject.h"
#include "third_party/python/Include/yoink.h"
/* clang-format off */

STATIC_YOINK("zip_uri_support");
PYTHON_YOINK("_bootlocale");
PYTHON_YOINK("_locale");
PYTHON_YOINK("encodings.aliases");
PYTHON_YOINK("encodings.latin_1");
PYTHON_YOINK("encodings.utf_8");
PYTHON_YOINK("launchpy");

extern char kLaunchPythonModuleName[]; /* generated by pyobj.com */
const struct _frozen *PyImport_FrozenModules = _PyImport_FrozenModules;
struct _inittab *PyImport_Inittab = _PyImport_Inittab;

int
LaunchPythonModule(const char *name)
{
    PyObject *mod, *runpy, *runmodule, *runargs, *result;
    if (!(runpy = PyImport_ImportModule("launchpy"))) {
        PyErr_Print();
        return 123;
    }
    if (!(runmodule = PyObject_GetAttrString(runpy, "run_module_as_main"))) {
        PyErr_Print();
        Py_DECREF(runpy);
        return 122;
    }
    if (!(mod = PyUnicode_DecodeUTF8Stateful(name, strlen(name), 0, 0))) {
        PyErr_Print();
        Py_DECREF(runpy);
        Py_DECREF(runmodule);
        return 121;
    }
    if (!(runargs = Py_BuildValue("(O)", mod))) {
        PyErr_Print();
        Py_DECREF(runpy);
        Py_DECREF(runmodule);
        Py_DECREF(mod);
        return 119;
    }
    if (!(result = PyObject_Call(runmodule, runargs, NULL))) {
        PyErr_Print();
    }
    Py_DECREF(runpy);
    Py_DECREF(runmodule);
    Py_DECREF(mod);
    Py_DECREF(runargs);
    if (!result) return 118;
    Py_DECREF(result);
    return 0;
}

int
main(int argc, char *argv[])
{
    size_t n;
    int i, sts;
    wchar_t *w;
    PyObject *a, *s;
    Py_FrozenFlag++;
    Py_NoSiteFlag++;
    /* Py_VerboseFlag++; */
    Py_NoUserSiteDirectory++;
    Py_IgnoreEnvironmentFlag++;
    Py_DontWriteBytecodeFlag++;
#if defined(Py_DEBUG) || defined(USE_TRACEMALLOC)
    _PyMem_SetupAllocators(Py_GETENV("PYTHONMALLOC"));
#else
    _PyMem_SetupAllocators(0);
#endif
    _PyRandom_Init();
    Py_Initialize();
    Py_LimitedPath();
    if (!(a = PyList_New(argc))) return 127;
    for (i = 0; i < argc; ++i) {
        if (!(w = utf8toutf32(argv[i], -1, &n))) return 126;
        if (!(s = PyUnicode_FromWideChar(w, n))) return 125;
        PyList_SetItem(a, i, s);
        free(w);
    }
    if (PySys_SetObject("argv", a)) return 124;
    sts = LaunchPythonModule(kLaunchPythonModuleName);
    if (Py_FinalizeEx() < 0) sts = 120;
    return sts;
}
