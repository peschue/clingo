// {{{ GPL License

// This file is part of gringo - a grounder for logic programs.
// Copyright Roland Kaminski

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// }}}

#include <gringo/control.hh>
#include <clingo.hh>

using namespace Gringo;

// {{{1 c interface

// {{{2 error handling

namespace {

clingo_solve_result_t convert(SolveResult r) {
    return static_cast<clingo_solve_result_t>(r.satisfiable()) |
           static_cast<clingo_solve_result_t>(r.interrupted()) * static_cast<clingo_solve_result_t>(clingo_solve_result_interrupted) |
           static_cast<clingo_solve_result_t>(r.exhausted()) * static_cast<clingo_solve_result_t>(clingo_solve_result_exhausted);
}

}

extern "C" inline char const *clingo_message_code_str(clingo_message_code_t code) {
    switch (code) {
        case clingo_error_success:               { return "success"; }
        case clingo_error_runtime:               { return "runtime error"; }
        case clingo_error_bad_alloc:             { return "bad allocation"; }
        case clingo_error_logic:                 { return "logic error"; }
        case clingo_error_unknown:               { return "unknown error"; }
        case clingo_warning_operation_undefined: { return "operation_undefined"; }
        case clingo_warning_atom_undefined:      { return "atom undefined"; }
        case clingo_warning_file_included:       { return "file included"; }
        case clingo_warning_variable_unbounded:  { return "variable unbounded"; }
        case clingo_warning_global_variable:     { return "global variable"; }
    }
    return "unknown message code";
}

// {{{2 value

extern "C" clingo_error_t clingo_signature_new(char const *name, uint32_t arity, bool sign, clingo_signature_t *ret) {
    GRINGO_CLINGO_TRY {
        *ret = Sig(name, arity, sign);
    } GRINGO_CLINGO_CATCH(nullptr);
}

extern "C" char const *clingo_signature_name(clingo_signature_t sig) {
    return static_cast<Sig&>(sig).name().c_str();
}

extern "C" uint32_t clingo_signature_arity(clingo_signature_t sig) {
    return static_cast<Sig&>(sig).arity();
}

extern "C" bool clingo_signature_sign(clingo_signature_t sig) {
    return static_cast<Sig&>(sig).sign();
}

extern "C" size_t clingo_signature_hash(clingo_signature_t sig) {
    return static_cast<Sig&>(sig).hash();
}

extern "C" bool clingo_signature_eq(clingo_signature_t a, clingo_signature_t b) {
    return static_cast<Sig&>(a) == static_cast<Sig&>(b);
}

extern "C" bool clingo_signature_lt(clingo_signature_t a, clingo_signature_t b) {
    return static_cast<Sig&>(a) < static_cast<Sig&>(b);
}


// {{{2 value

extern "C" void clingo_symbol_new_num(int num, clingo_symbol_t *val) {
    *val = Symbol::createNum(num);
}

extern "C" void clingo_symbol_new_sup(clingo_symbol_t *val) {
    *val = Symbol::createSup();
}

extern "C" void clingo_symbol_new_inf(clingo_symbol_t *val) {
    *val = Symbol::createInf();
}

extern "C" clingo_error_t clingo_symbol_new_str(char const *str, clingo_symbol_t *val) {
    GRINGO_CLINGO_TRY {
        *val = Symbol::createStr(str);
    } GRINGO_CLINGO_CATCH(nullptr);
}

extern "C" clingo_error_t clingo_symbol_new_id(char const *id, bool sign, clingo_symbol_t *val) {
    GRINGO_CLINGO_TRY {
        *val = Symbol::createId(id, sign);
    } GRINGO_CLINGO_CATCH(nullptr);
}

extern "C" clingo_error_t clingo_symbol_new_fun(char const *name, clingo_symbol_span_t args, bool sign, clingo_symbol_t *val) {
    GRINGO_CLINGO_TRY {
        *val = Symbol::createFun(name, SymSpan{static_cast<Symbol const *>(args.first), args.size}, sign);
    } GRINGO_CLINGO_CATCH(nullptr);
}

extern "C" clingo_error_t clingo_symbol_num(clingo_symbol_t val, int *num) {
    GRINGO_CLINGO_TRY {
        clingo_expect(static_cast<Symbol&>(val).type() == SymbolType::Num);
        *num = static_cast<Symbol&>(val).num();
    } GRINGO_CLINGO_CATCH(nullptr);
}

extern "C" clingo_error_t clingo_symbol_name(clingo_symbol_t val, char const **name) {
    GRINGO_CLINGO_TRY {
        clingo_expect(static_cast<Symbol&>(val).type() == SymbolType::Fun);
        *name = static_cast<Symbol&>(val).name().c_str();
    } GRINGO_CLINGO_CATCH(nullptr);
}

extern "C" clingo_error_t clingo_symbol_string(clingo_symbol_t val, char const **str) {
    GRINGO_CLINGO_TRY {
        clingo_expect(static_cast<Symbol&>(val).type() == SymbolType::Str);
        *str = static_cast<Symbol&>(val).string().c_str();
    } GRINGO_CLINGO_CATCH(nullptr);
}

extern "C" clingo_error_t clingo_symbol_sign(clingo_symbol_t val, bool *sign) {
    GRINGO_CLINGO_TRY {
        clingo_expect(static_cast<Symbol&>(val).type() == SymbolType::Fun);
        *sign = static_cast<Symbol&>(val).sign();
        return clingo_error_success;
    } GRINGO_CLINGO_CATCH(nullptr);
}

extern "C" clingo_error_t clingo_symbol_args(clingo_symbol_t val, clingo_symbol_span_t *args) {
    GRINGO_CLINGO_TRY {
        clingo_expect(static_cast<Symbol&>(val).type() == SymbolType::Fun);
        auto ret = static_cast<Symbol&>(val).args();
        *args = clingo_symbol_span_t{ret.first, ret.size};
    } GRINGO_CLINGO_CATCH(nullptr);
}

extern "C" clingo_symbol_type_t clingo_symbol_type(clingo_symbol_t val) {
    return static_cast<clingo_symbol_type_t>(static_cast<Symbol&>(val).type());
}

extern "C" clingo_error_t clingo_symbol_to_string(clingo_symbol_t val, char *ret, size_t *n) {
    GRINGO_CLINGO_TRY {
        if (!n) { throw std::invalid_argument("size must not be null"); }
        if (!ret) {
            Gringo::CountStream cs;
            static_cast<Symbol&>(val).print(cs);
            *n = cs.count() + 1;
        }
        else {
            if (*n < 1) { throw std::length_error("not enough space"); }
            Gringo::ArrayStream as(ret, *n - 1);
            static_cast<Symbol&>(val).print(as);
        }
    } GRINGO_CLINGO_CATCH(nullptr);
}

extern "C" bool clingo_symbol_eq(clingo_symbol_t a, clingo_symbol_t b) {
    return static_cast<Symbol&>(a) == static_cast<Symbol&>(b);
}

extern "C" bool clingo_symbol_lt(clingo_symbol_t a, clingo_symbol_t b) {
    return static_cast<Symbol&>(a) < static_cast<Symbol&>(b);
}

extern "C" size_t clingo_symbol_hash(clingo_symbol_t sym) {
    return static_cast<Symbol&>(sym).hash();
}

// {{{2 symbolic atoms

extern "C" clingo_symbolic_atom_range_t clingo_symbolic_atoms_iter(clingo_symbolic_atoms_t *dom, clingo_signature_t *sig) {
    return sig ? dom->iter(static_cast<Sig&>(*sig)) : dom->iter();
}

extern "C" clingo_symbolic_atom_range_t clingo_symbolic_atoms_lookup(clingo_symbolic_atoms_t *dom, clingo_symbol_t atom) {
    return dom->lookup(static_cast<Symbol&>(atom));
}

extern "C" clingo_error_t clingo_symbolic_atoms_signatures(clingo_symbolic_atoms_t *dom, clingo_signature_t *ret, size_t *n) {
    GRINGO_CLINGO_TRY {
        // TODO: implement matching C++ functions ...
        auto sigs = dom->signatures();
        if (!n) { throw std::invalid_argument("size must be non-null"); }
        if (!ret) { *n = sigs.size(); }
        else {
            if (*n < sigs.size()) { throw std::length_error("not enough space"); }
            for (auto &sig : sigs) { *ret++ = sig; }
        }
    }
    GRINGO_CLINGO_CATCH(&dom->owner().logger());
}

extern "C" size_t clingo_symbolic_atoms_length(clingo_symbolic_atoms_t *dom) {
    return dom->length();
}

extern "C" clingo_error_t clingo_symbolic_atoms_atom(clingo_symbolic_atoms_t *dom, clingo_symbolic_atom_range_t atm, clingo_symbol_t *sym) {
    GRINGO_CLINGO_TRY { *sym = dom->atom(atm); }
    GRINGO_CLINGO_CATCH(&dom->owner().logger());
}

extern "C" clingo_error_t clingo_symbolic_atoms_literal(clingo_symbolic_atoms_t *dom, clingo_symbolic_atom_range_t atm, lit_t *lit) {
    GRINGO_CLINGO_TRY { *lit = dom->literal(atm); }
    GRINGO_CLINGO_CATCH(&dom->owner().logger());
}

extern "C" clingo_error_t clingo_symbolic_atoms_fact(clingo_symbolic_atoms_t *dom, clingo_symbolic_atom_range_t atm, bool *fact) {
    GRINGO_CLINGO_TRY { *fact = dom->fact(atm); }
    GRINGO_CLINGO_CATCH(&dom->owner().logger());
}

extern "C" clingo_error_t clingo_symbolic_atoms_external(clingo_symbolic_atoms_t *dom, clingo_symbolic_atom_range_t atm, bool *external) {
    GRINGO_CLINGO_TRY { *external = dom->external(atm); }
    GRINGO_CLINGO_CATCH(&dom->owner().logger());
}

extern "C" clingo_error_t clingo_symbolic_atoms_next(clingo_symbolic_atoms_t *dom, clingo_symbolic_atom_range_t atm, clingo_symbolic_atom_range_t *next) {
    GRINGO_CLINGO_TRY { *next = dom->next(atm); }
    GRINGO_CLINGO_CATCH(&dom->owner().logger());
}

extern "C" clingo_error_t clingo_symbolic_atoms_valid(clingo_symbolic_atoms_t *dom, clingo_symbolic_atom_range_t atm, bool *valid) {
    GRINGO_CLINGO_TRY { *valid = dom->valid(atm); }
    GRINGO_CLINGO_CATCH(&dom->owner().logger());
}

// {{{2 model

extern "C" bool clingo_model_contains(clingo_model_t *m, clingo_symbol_t atom) {
    return m->contains(static_cast<Symbol &>(atom));
}

extern "C" clingo_error_t clingo_model_atoms(clingo_model_t *m, clingo_show_type_t show, clingo_symbol_t *ret, size_t *n) {
    GRINGO_CLINGO_TRY {
        // TODO: implement matching C++ functions ...
        SymSpan atoms = m->atoms(show);
        if (!n) { throw std::invalid_argument("size must be non-null"); }
        if (!ret) { *n = atoms.size; }
        else {
            if (*n < atoms.size) { throw std::length_error("not enough space"); }
            for (auto it = atoms.first, ie = it + atoms.size; it != ie; ++it) { *ret++ = *it; }
        }
    } GRINGO_CLINGO_CATCH(&m->owner().logger());
}

// {{{2 solve_iter

struct clingo_solve_iter : SolveIter { };

extern "C" clingo_error_t clingo_solve_iter_next(clingo_solve_iter_t *it, clingo_model **m) {
    GRINGO_CLINGO_TRY {
        *m = static_cast<clingo_model*>(const_cast<Model*>(it->next()));
    } GRINGO_CLINGO_CATCH(&it->owner().logger());
}

extern "C" clingo_error_t clingo_solve_iter_get(clingo_solve_iter_t *it, clingo_solve_result_t *ret) {
    GRINGO_CLINGO_TRY {
        *ret = convert(it->get().satisfiable());
    } GRINGO_CLINGO_CATCH(&it->owner().logger());
}

extern "C" clingo_error_t clingo_solve_iter_close(clingo_solve_iter_t *it) {
    GRINGO_CLINGO_TRY {
        it->close();
    } GRINGO_CLINGO_CATCH(&it->owner().logger());
}

 // {{{2 control

extern "C" clingo_error_t clingo_control_new(clingo_module_t *mod, clingo_string_span_t args, clingo_logger_t *logger, void *data, unsigned message_limit, clingo_control_t **ctl) {
    GRINGO_CLINGO_TRY {
        // NOTE: nullptr sentinel required by program options library
        // TODO: ask Benny about possible removal
        std::vector<char const *> argVec;
        for (auto it = args.first, ie = it + args.size; it != ie; ++it) {
            argVec.emplace_back(*it);
        }
        argVec.push_back(nullptr);
        *ctl = mod->newControl(args.size, argVec.data(), logger ? [logger, data](clingo_message_code_t code, char const *msg) { logger(code, msg, data); } : Gringo::Logger::Printer(nullptr), message_limit);
    } GRINGO_CLINGO_CATCH(nullptr);
}

extern "C" void clingo_control_free(clingo_control_t *ctl) {
    delete ctl;
}

extern "C" clingo_error_t clingo_control_add(clingo_control_t *ctl, char const *name, clingo_string_span_t params, char const *part) {
    GRINGO_CLINGO_TRY {
        FWStringVec p;
        for (char const * const *it = params.first, * const *ie = it + params.size; it != ie; ++it) { p.emplace_back(*it); }
        ctl->add(name, p, part);
    } GRINGO_CLINGO_CATCH(&ctl->logger());
}

namespace {

struct ClingoContext : Context {
    ClingoContext(clingo_control_t *ctl, clingo_ground_callback_t *cb, void *data)
    : ctl(ctl)
    , cb(cb)
    , data(data) {}

    bool callable(String) const override {
        return cb;
    }

    SymVec call(Location const &loc, String name, SymSpan args) override {
        assert(cb);
        clingo_location_t loc_c{loc.beginFilename.c_str(), loc.endFilename.c_str(), loc.beginLine, loc.endLine, loc.beginColumn, loc.endColumn};
        clingo_symbol_span_t args_c;
        args_c = { args.first, args.size };
        auto err = cb(loc_c, name.c_str(), args_c, data, [](clingo_symbol_span_t ret_c, void *data) -> clingo_error_t {
            auto t = static_cast<ClingoContext*>(data);
            GRINGO_CLINGO_TRY {
                for (auto it = ret_c.first, ie = it + ret_c.size; it != ie; ++it) {
                    t->ret.emplace_back(static_cast<Symbol const &>(*it));
                }
            } GRINGO_CLINGO_CATCH(&t->ctl->logger());
        }, static_cast<void*>(this));
        if (err != 0) { throw ClingoError(err); }
        return std::move(ret);
    }
    virtual ~ClingoContext() noexcept = default;

    clingo_control_t *ctl;
    clingo_ground_callback_t *cb;
    void *data;
    SymVec ret;
};

}

extern "C" clingo_error_t clingo_control_ground(clingo_control_t *ctl, clingo_part_span_t vec, clingo_ground_callback_t *cb, void *data) {
    GRINGO_CLINGO_TRY {
        Control::GroundVec gv;
        gv.reserve(vec.size);
        for (auto it = vec.first, ie = it + vec.size; it != ie; ++it) {
            SymVec params;
            params.reserve(it->params.size);
            for (auto jt = it->params.first, je = jt + it->params.size; jt != je; ++jt) {
                params.emplace_back(static_cast<Symbol const &>(*jt));
            }
            gv.emplace_back(it->name, params);
        }
        ClingoContext cctx(ctl, cb, data);
        ctl->ground(gv, cb ? &cctx : nullptr);
    } GRINGO_CLINGO_CATCH(&ctl->logger());
}

namespace {

Control::Assumptions toAss(clingo_symbolic_literal_span_t assumptions) {
    Control::Assumptions ass;
    for (auto it = assumptions.first, ie = it + assumptions.size; it != ie; ++it) {
        ass.emplace_back(static_cast<Symbol const>(it->atom), !it->sign);
    }
    return ass;
}

}

extern "C" clingo_error_t clingo_control_solve(clingo_control_t *ctl, clingo_model_handler_t *model_handler, void *data, clingo_symbolic_literal_span_t assumptions, clingo_solve_result_t *ret) {
    GRINGO_CLINGO_TRY {
        *ret = static_cast<clingo_solve_result_t>(ctl->solve([model_handler, data](Model const &m) {
            bool ret;
            auto err = model_handler(static_cast<clingo_model*>(const_cast<Model*>(&m)), data, &ret);
            if (err != 0) { throw ClingoError(err); }
            return ret;
        }, toAss(assumptions)));
    } GRINGO_CLINGO_CATCH(&ctl->logger());
}

extern "C" clingo_error_t clingo_control_solve_iter(clingo_control_t *ctl, clingo_symbolic_literal_span_t assumptions, clingo_solve_iter_t **it) {
    GRINGO_CLINGO_TRY {
        *it = static_cast<clingo_solve_iter_t*>(ctl->solveIter(toAss(assumptions)));
    } GRINGO_CLINGO_CATCH(&ctl->logger());
}

extern "C" clingo_error_t clingo_control_assign_external(clingo_control_t *ctl, clingo_symbol_t atom, clingo_truth_value_t value) {
    GRINGO_CLINGO_TRY {
        ctl->assignExternal(static_cast<Symbol const &>(atom), static_cast<Potassco::Value_t>(value));
    } GRINGO_CLINGO_CATCH(&ctl->logger());
}

extern "C" clingo_error_t clingo_control_release_external(clingo_control_t *ctl, clingo_symbol_t atom) {
    GRINGO_CLINGO_TRY {
        ctl->assignExternal(static_cast<Symbol const &>(atom), Potassco::Value_t::Release);
    } GRINGO_CLINGO_CATCH(&ctl->logger());
}

extern "C" clingo_error_t clingo_control_parse(clingo_control_t *ctl, char const *program, clingo_ast_callback_t *cb, void *data) {
    GRINGO_CLINGO_TRY {
        ctl->parse(program, [data, cb](clingo_ast const &ast) {
            auto ret = cb(&ast, data);
            if (ret != 0) { throw ClingoError(ret); }
        });
    } GRINGO_CLINGO_CATCH(&ctl->logger());
}

extern "C" clingo_error_t clingo_control_add_ast(clingo_control_t *ctl, clingo_add_ast_callback_t *cb, void *data) {
    GRINGO_CLINGO_TRY {
        ctl->add([ctl, data, cb](std::function<void (clingo_ast const &)> f) {
            auto ref = std::make_pair(f, ctl);
            using RefType = decltype(ref);
            auto ret = cb(data, [](clingo_ast_t const *ast, void *data) -> clingo_error_t {
                auto &ref = *static_cast<RefType*>(data);
                GRINGO_CLINGO_TRY {
                    ref.first(static_cast<clingo_ast const &>(*ast));
                } GRINGO_CLINGO_CATCH(&ref.second->logger());
            }, static_cast<void*>(&ref));
            if (ret != 0) { throw ClingoError(ret); }
        });
    } GRINGO_CLINGO_CATCH(&ctl->logger());
}

extern "C" clingo_error_t clingo_control_symbolic_atoms(clingo_control_t *ctl, clingo_symbolic_atoms_t **ret) {
    GRINGO_CLINGO_TRY { *ret = &ctl->getDomain(); }
    GRINGO_CLINGO_CATCH(&ctl->logger());
}

// }}}2

namespace Clingo {

// {{{1 c++ interface

// {{{2 error handling

} namespace Gringo {

void handleError(clingo_error_t code, std::exception_ptr *exc) {
    switch (code) {
        case clingo_error_success:   { break; }
        case clingo_error_fatal:     { throw std::runtime_error("fatal error"); }
        case clingo_error_runtime:   { throw std::runtime_error("runtime error"); }
        case clingo_error_logic:     { throw std::logic_error("logic error"); }
        case clingo_error_bad_alloc: { throw std::bad_alloc(); }
        case clingo_error_unknown:   {
            if (exc && *exc) { std::rethrow_exception(*exc); }
            throw std::logic_error("unknown error");
        }
    }
}

} namespace Clingo {

// {{{2 signature

Signature::Signature(char const *name, uint32_t arity, bool sign) {
    handleError(clingo_signature_new(name, arity, sign, this));
}

char const *Signature::name() const {
    return clingo_signature_name(*this);
}

uint32_t Signature::arity() const {
    return clingo_signature_arity(*this);
}

bool Signature::sign() const {
    return clingo_signature_sign(*this);
}


size_t Signature::hash() const {
    return clingo_signature_hash(*this);
}

bool operator==(Signature a, Signature b) { return  clingo_signature_eq(a, b); }
bool operator!=(Signature a, Signature b) { return !clingo_signature_eq(a, b); }
bool operator< (Signature a, Signature b) { return  clingo_signature_lt(a, b); }
bool operator<=(Signature a, Signature b) { return !clingo_signature_lt(b, a); }
bool operator> (Signature a, Signature b) { return  clingo_signature_lt(b, a); }
bool operator>=(Signature a, Signature b) { return !clingo_signature_lt(a, b); }

// {{{2 symbol

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

Symbol::Symbol() {
    clingo_symbol_new_num(0, this);
}

Symbol Num(int num) {
    clingo_symbol_t sym;
    clingo_symbol_new_num(num, &sym);
    return static_cast<Symbol&>(sym);
}

Symbol Sup() {
    clingo_symbol_t sym;
    clingo_symbol_new_sup(&sym);
    return static_cast<Symbol&>(sym);
}

Symbol Inf() {
    clingo_symbol_t sym;
    clingo_symbol_new_inf(&sym);
    return static_cast<Symbol&>(sym);
}

Symbol Str(char const *str) {
    clingo_symbol_t sym;
    handleError(clingo_symbol_new_str(str, &sym));
    return static_cast<Symbol&>(sym);
}

Symbol Id(char const *id, bool sign) {
    clingo_symbol_t sym;
    handleError(clingo_symbol_new_id(id, sign, &sym));
    return static_cast<Symbol&>(sym);
}

Symbol Fun(char const *name, SymSpan args, bool sign) {
    clingo_symbol_t sym;
    handleError(clingo_symbol_new_fun(name, args, sign, &sym));
    return static_cast<Symbol&>(sym);
}

int Symbol::num() const {
    int ret;
    handleError(clingo_symbol_num(*this, &ret));
    return ret;
}

char const *Symbol::name() const {
    char const *ret;
    handleError(clingo_symbol_name(*this, &ret));
    return ret;
}

char const *Symbol::string() const {
    char const *ret;
    handleError(clingo_symbol_string(*this, &ret));
    return ret;
}

bool Symbol::sign() const {
    bool ret;
    handleError(clingo_symbol_sign(*this, &ret));
    return ret;
}

SymSpan Symbol::args() const {
    SymSpan ret;
    handleError(clingo_symbol_args(*this, &ret));
    return ret;
}

SymbolType Symbol::type() const {
    return static_cast<SymbolType>(clingo_symbol_type(*this));
}

#define CLINGO_CALLBACK_TRY try
#define CLINGO_CALLBACK_CATCH(ref) catch (...){ (ref) = std::current_exception(); return clingo_error_unknown; } return clingo_error_success

std::string Symbol::to_string() const {
    std::string ret;
    size_t n;
    handleError(clingo_symbol_to_string(*this, nullptr, &n));
    ret.resize(n-1);
    handleError(clingo_symbol_to_string(*this, const_cast<char*>(ret.data()), &n));
    return ret;
}

size_t Symbol::hash() const {
    return clingo_symbol_hash(*this);
}

std::ostream &operator<<(std::ostream &out, Symbol sym) {
    out << sym.to_string();
    return out;
}

bool operator==(Symbol a, Symbol b) { return  clingo_symbol_eq(a, b); }
bool operator!=(Symbol a, Symbol b) { return !clingo_symbol_eq(a, b); }
bool operator< (Symbol a, Symbol b) { return  clingo_symbol_lt(a, b); }
bool operator<=(Symbol a, Symbol b) { return !clingo_symbol_lt(b, a); }
bool operator> (Symbol a, Symbol b) { return  clingo_symbol_lt(b, a); }
bool operator>=(Symbol a, Symbol b) { return !clingo_symbol_lt(a, b); }

// {{{2 symbolic atoms

Symbol SymbolicAtom::symbol() const {
    Symbol ret;
    clingo_symbolic_atoms_atom(atoms_, range_, &ret);
    return ret;
}

lit_t SymbolicAtom::literal() const {
    lit_t ret;
    clingo_symbolic_atoms_literal(atoms_, range_, &ret);
    return ret;
}

bool SymbolicAtom::fact() const {
    bool ret;
    clingo_symbolic_atoms_fact(atoms_, range_, &ret);
    return ret;
}

bool SymbolicAtom::external() const {
    bool ret;
    clingo_symbolic_atoms_external(atoms_, range_, &ret);
    return ret;
}

SymbolicAtomRange &SymbolicAtomRange::operator++() {
    clingo_symbolic_atom_range_t range;
    handleError(clingo_symbolic_atoms_next(atoms_, range_, &range));
    range_ = range;
    return *this;
}

SymbolicAtomRange::operator bool() const {
    bool ret;
    handleError(clingo_symbolic_atoms_valid(atoms_, range_, &ret));
    return ret;
}

SymbolicAtomRange SymbolicAtoms::range() {
    return {atoms_, clingo_symbolic_atoms_iter(atoms_, nullptr) };
}

SymbolicAtomRange SymbolicAtoms::range(Signature sig) {
    return {atoms_, clingo_symbolic_atoms_iter(atoms_, &sig) };
}

SymbolicAtom SymbolicAtoms::lookup(Symbol atom) {
    return {atoms_, clingo_symbolic_atoms_lookup(atoms_, atom) };
}

std::vector<Signature> SymbolicAtoms::signatures() {
    size_t n;
    clingo_symbolic_atoms_signatures(atoms_, nullptr, &n);
    Signature sig("", 0);
    std::vector<Signature> ret;
    ret.resize(n, sig);
    clingo_symbolic_atoms_signatures(atoms_, ret.data(), &n);
    return ret;
}

size_t SymbolicAtoms::length() const {
    return clingo_symbolic_atoms_length(atoms_);
}

// {{{2 model

Model::Model(clingo_model_t *model)
: model_(model) { }

bool Model::contains(Symbol atom) const {
    return clingo_model_contains(model_, atom);
}

SymVec Model::atoms(ShowType show) const {
    SymVec ret;
    size_t n;
    handleError(clingo_model_atoms(model_, show, nullptr, &n));
    ret.resize(n);
    handleError(clingo_model_atoms(model_, show, ret.data(), &n));
    return ret;
}

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

// {{{2 solve iter

SolveIter::SolveIter()
: iter_(nullptr) { }

SolveIter::SolveIter(clingo_solve_iter_t *it)
: iter_(it) { }

SolveIter::SolveIter(SolveIter &&it)
: iter_(nullptr) { std::swap(iter_, it.iter_); }

SolveIter &SolveIter::operator=(SolveIter &&it) {
    std::swap(iter_, it.iter_);
    return *this;
}

Model SolveIter::next() {
    clingo_model_t *m = nullptr;
    if (iter_) { handleError(clingo_solve_iter_next(iter_, &m)); }
    return m;
}

SolveResult SolveIter::get() {
    clingo_solve_result_t ret = 0;
    if (iter_) { handleError(clingo_solve_iter_get(iter_, &ret)); }
    return ret;
}

void SolveIter::close() {
    if (iter_) {
        clingo_solve_iter_close(iter_);
        iter_ = nullptr;
    }
}

// {{{2 control

Control::Control(clingo_control_t *ctl)
: ctl_(ctl) { }

Control::~Control() noexcept {
    clingo_control_free(ctl_);
}

void Control::add(char const *name, StringSpan params, char const *part) {
    handleError(clingo_control_add(ctl_, name, params, part));
}

void Control::ground(PartSpan parts, GroundCallback cb) {
    using Data = std::pair<GroundCallback&, std::exception_ptr>;
    Data data(cb, nullptr);
    handleError(clingo_control_ground(ctl_, parts,
        [](clingo_location_t loc, char const *name, clingo_symbol_span_t args, void *data, clingo_symbol_span_callback_t *cb, void *cbdata) -> clingo_error_t {
            auto &d = *static_cast<Data*>(data);
            CLINGO_CALLBACK_TRY {
                if (d.first) {
                    struct Ret { clingo_error_t ret; };
                    try {
                        d.first(loc, name, args, [cb, cbdata](SymSpan symret) {
                            clingo_error_t ret = cb(symret, cbdata);
                            if (ret != clingo_error_success) { throw Ret { ret }; }
                        });
                    }
                    catch (Ret e) { return e.ret; }
                }
            }
            CLINGO_CALLBACK_CATCH(d.second);
        }, &data), &data.second);
}

Control::operator clingo_control_t*() const { return ctl_; }

SolveResult Control::solve(ModelHandler mh, SymbolicLiteralSpan assumptions) {
    clingo_solve_result_t ret;
    using Data = std::pair<ModelHandler&, std::exception_ptr>;
    Data data(mh, nullptr);
    clingo_control_solve(ctl_, [](clingo_model_t*m, void *data, bool *ret) -> clingo_error_t {
        auto &d = *static_cast<Data*>(data);
        CLINGO_CALLBACK_TRY { *ret = d.first(m); }
        CLINGO_CALLBACK_CATCH(d.second);
    }, &data, assumptions, &ret);
    return ret;
}

SolveIter Control::solve_iter(SymbolicLiteralSpan assumptions) {
    clingo_solve_iter_t *it;
    clingo_control_solve_iter(ctl_, assumptions, &it);
    return it;
}

void Control::assign_external(Symbol atom, TruthValue value) {
    handleError(clingo_control_assign_external(ctl_, atom, static_cast<clingo_truth_value_t>(value)));
}

void Control::release_external(Symbol atom) {
    handleError(clingo_control_release_external(ctl_, atom));
}

SymbolicAtoms Control::symbolic_atoms() {
    clingo_symbolic_atoms_t *ret;
    clingo_control_symbolic_atoms(ctl_, &ret);
    return ret;
}

// }}}2

// }}}1

} // namespace Clingo

