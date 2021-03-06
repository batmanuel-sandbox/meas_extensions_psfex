// -*- lsst-C++ -*-
#include "lsst/meas/extensions/psfex/prefs.hh"
#include "lsst/daf/base.h"

namespace lsst { namespace meas { namespace extensions { namespace psfex {

Prefs::Prefs(std::string const& filename, lsst::daf::base::PropertySet const* values
            ) : _command_line(0) {
    char *cfilename = const_cast<char *>(filename.c_str()); // const_cast as PSFEX isn't careful about const
    int const narg = values->nameCount();
    if (narg == 0) {
        readprefs(cfilename, 0x0, 0x0, narg);
    } else {
        std::vector<char *> argkey(narg);
        std::vector<char *> argval(narg);
        std::vector<std::string> const names = values->paramNames();
        // Next, decalre a vector to store string values. These strings must
        // live long enough such that pointers to them (returned by c_str) stay
        // valid until the readprefs function can read their values. Once this
        // happens, the srings and pointers are no longer needed, and will be
        // cleaned up when the vector goes out of scope
        std::vector<std::string> paramvalues(narg);

        for (int i = 0; i != narg; ++i) {
            std::string const& name = names[i];
            argkey[i] = const_cast<char *>(name.c_str());
            paramvalues[i] = values->getAsString(name);
            argval[i] = const_cast<char *>(paramvalues[i].c_str());
        }
    
        readprefs(cfilename, &argkey[0], &argval[0], narg);
    }

    for (int i = 0; i != prefs.ncontext_name; ++i) {
        _context_names.push_back(prefs.context_name[i]);
    }
    for (int i = 0; i != prefs.ncontext_group; ++i) {
        _context_groups.push_back(prefs.context_group[i]);
    }
    for (int i = 0; i != prefs.ngroup_deg; ++i) {
        _group_degs.push_back(prefs.group_deg[i]);
    }
}

Prefs::~Prefs()
{
    delete[] _command_line;
}

void
Prefs::setCommandLine(std::vector<std::string> const& argv)
{
    prefs.ncommand_line = argv.size();
    _command_line = new char const*[prefs.ncommand_line + 1];
    int i;
    for (i = 0; i != prefs.ncommand_line; ++i) {
        _command_line[i] = argv[i].c_str();
    }
    _command_line[i] = 0;
    prefs.command_line = const_cast<char **>(_command_line);
}

void
Prefs::addCatalog(std::string const& filename) {
    if (prefs.ncat >= MAXFILE) {
        throw LSST_EXCEPT(lsst::pex::exceptions::LengthError, "Too many input catalogues");
    }
    _catalogs.push_back(filename);
    prefs.incat_name[prefs.ncat++] = const_cast<char *>((_catalogs.end() - 1)->c_str());
}

}}}}
