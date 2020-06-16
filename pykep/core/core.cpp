/*****************************************************************************
 *   Copyright (C) 2004-2018 The pagmo development team,                     *
 *   Advanced Concepts Team (ACT), European Space Agency (ESA)               *
 *   http://apps.sourceforge.net/mediawiki/pagmo                             *
 *   http://apps.sourceforge.net/mediawiki/pagmo/index.php?title=Developers  *
 *   http://apps.sourceforge.net/mediawiki/pagmo/index.php?title=Credits     *
 *   act@esa.int                                                             *
 *                                                                           *
 *   This program is free software; you can redistribute it and/or modify    *
 *   it under the terms of the GNU General Public License as published by    *
 *   the Free Software Foundation; either version 3 of the License, or       *
 *   (at your option) any later version.                                     *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 *   GNU General Public License for more details.                            *
 *                                                                           *
 *   You should have received a copy of the GNU General Public License       *
 *   along with this program; if not, write to the                           *
 *   Free Software Foundation, Inc.,                                         *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.               *
 *****************************************************************************/

// NOTE: the order of inclusion in the first two items here is forced by these
// two issues:
// http://mail.python.org/pipermail/python-list/2004-March/907592.html
// http://mail.python.org/pipermail/new-bugs-announce/2011-March/010395.html
#if defined(_WIN32)
#include <Python.h>
#include <cmath>
#else
#include <Python.h>
#include <cmath>
#endif

#if PY_MAJOR_VERSION < 3
#error Minimum supported Python version is 2.6.
#endif

#include <boost/math/constants/constants.hpp>
#include <boost/python.hpp>
#include <boost/python/class.hpp>
#include <boost/python/converter/registry.hpp>
#include <boost/python/copy_const_reference.hpp>
#include <boost/python/def.hpp>
#include <boost/python/docstring_options.hpp>
#include <boost/python/enum.hpp>
#include <boost/python/module.hpp>
#include <boost/python/operators.hpp>
#include <boost/python/overloads.hpp>
#include <boost/python/register_ptr_to_python.hpp>
#include <boost/python/self.hpp>
#include <boost/utility.hpp>

#include "../boost_python_container_conversions.h"
#include "../utils.h"
#include "core_docstrings.hpp"
#include <keplerian_toolbox/keplerian_toolbox.hpp>

using namespace boost::python;

static inline tuple par2ic_wrapper(const kep_toolbox::array6D &E, const double &mu)
{
    kep_toolbox::array3D r0, v0;
    kep_toolbox::par2ic(E, mu, r0, v0);
    return boost::python::make_tuple(r0, v0);
}

static inline kep_toolbox::array6D ic2par_wrapper(const kep_toolbox::array3D &r0, const kep_toolbox::array3D &v0,
                                                  const double &mu)
{
    kep_toolbox::array6D E;
    kep_toolbox::ic2par(r0, v0, mu, E);
    return E;
}

static inline kep_toolbox::array6D par2eq_wrapper(const kep_toolbox::array6D &E, const bool retrograde = false)
{
    kep_toolbox::array6D EQ;
    kep_toolbox::par2eq(EQ, E, retrograde);
    return EQ;
}

static inline kep_toolbox::array6D eq2par_wrapper(const kep_toolbox::array6D &EQ, const bool retrograde = false)
{
    kep_toolbox::array6D E;
    kep_toolbox::eq2par(E, EQ, retrograde);
    return E;
}

static inline tuple eq2ic_wrapper(const kep_toolbox::array6D &EQ, const double &mu, const bool retrograde = false)
{
    kep_toolbox::array3D r0, v0;
    kep_toolbox::eq2ic(EQ, mu, r0, v0, retrograde);
    return boost::python::make_tuple(r0, v0);
}

static inline kep_toolbox::array6D ic2eq_wrapper(const kep_toolbox::array3D &r0, const kep_toolbox::array3D &v0,
                                                 const double &mu, const bool retrograde = false)
{
    kep_toolbox::array6D EQ;
    kep_toolbox::ic2eq(r0, v0, mu, EQ, retrograde);
    return EQ;
}

static inline tuple closest_distance_wrapper(const kep_toolbox::array3D &r0, const kep_toolbox::array3D &v0,
                                             const kep_toolbox::array3D &r1, const kep_toolbox::array3D &v1,
                                             const double &mu)
{
    double min_d, ra;
    kep_toolbox::closest_distance(min_d, ra, r0, v0, r1, v1, mu);
    return boost::python::make_tuple(min_d, ra);
}

static inline tuple propagate_lagrangian_wrapper(const kep_toolbox::array3D &r0, const kep_toolbox::array3D &v0,
                                                 const double &t, const double &mu)
{
    kep_toolbox::array3D r(r0), v(v0);
    kep_toolbox::propagate_lagrangian_u(r, v, t, mu);
    return boost::python::make_tuple(r, v);
}

static inline tuple propagate_taylor_wrapper(const kep_toolbox::array3D &r0, const kep_toolbox::array3D &v0,
                                             const double &m0, const kep_toolbox::array3D &u, const double &t,
                                             const double &mu, const double &veff, const int &log10tolerance,
                                             const int &log10rtolerance)
{
    kep_toolbox::array3D r(r0), v(v0);
    double m(m0);
    kep_toolbox::propagate_taylor(r, v, m, u, t, mu, veff, log10tolerance, log10rtolerance);
    return boost::python::make_tuple(kep_toolbox::array3D(r), kep_toolbox::array3D(v), double(m));
}

static inline tuple propagate_taylor_disturbance_wrapper(const kep_toolbox::array3D &r0, const kep_toolbox::array3D &v0,
                                                         const double &m0, const kep_toolbox::array3D &thrust,
                                                         const kep_toolbox::array3D &disturbance, const double &t,
                                                         const double &mu, const double &veff,
                                                         const int &log10tolerance, const int &log10rtolerance)
{
    kep_toolbox::array3D r(r0), v(v0);
    double m(m0);
    kep_toolbox::propagate_taylor_disturbance(r, v, m, thrust, disturbance, t, mu, veff, log10tolerance,
                                              log10rtolerance);
    return boost::python::make_tuple(kep_toolbox::array3D(r), kep_toolbox::array3D(v), double(m));
}

static inline tuple propagate_taylor_J2_wrapper(const kep_toolbox::array3D &r0, const kep_toolbox::array3D &v0,
                                                const double &m0, const kep_toolbox::array3D &u, const double &t,
                                                const double &mu, const double &veff, const double &J2RG2,
                                                const int &log10tolerance, const int &log10rtolerance)
{
    kep_toolbox::array3D r(r0), v(v0);
    double m(m0);
    kep_toolbox::propagate_taylor_J2(r, v, m, u, t, mu, veff, J2RG2, log10tolerance, log10rtolerance);
    return boost::python::make_tuple(kep_toolbox::array3D(r), kep_toolbox::array3D(v), double(m));
}

static inline tuple propagate_taylor_s_wrapper(const kep_toolbox::array3D &r0, const kep_toolbox::array3D &v0,
                                               const double &m0, const kep_toolbox::array3D &u, const double &s,
                                               const double &mu = 1.0, const double &veff = 1.0, const double &c = 1.0,
                                               const double &alpha = 1.5, const int &log10tolerance = -10,
                                               const int &log10rtolerance = -10)
{
    kep_toolbox::array3D r(r0), v(v0);
    double m(m0);
    double dt(0);
    kep_toolbox::propagate_taylor_s(r, v, m, dt, u, s, mu, veff, c, alpha, log10tolerance, log10rtolerance);
    return boost::python::make_tuple(kep_toolbox::array3D(r), kep_toolbox::array3D(v), double(m), double(dt));
}

static inline tuple fb_con_wrapper(const kep_toolbox::array3D &vin_rel, const kep_toolbox::array3D &vout_rel,
                                   const kep_toolbox::planet::base &pl)
{
    double eq(0), ineq(0);
    kep_toolbox::fb_con(eq, ineq, vin_rel, vout_rel, pl);
    return boost::python::make_tuple(double(eq), double(ineq));
}

static inline kep_toolbox::array3D fb_prop_wrapper(const kep_toolbox::array3D &v_in, const kep_toolbox::array3D &v_pla,
                                                   const double &rp, const double &beta, const double &mu)
{
    kep_toolbox::array3D retval;
    kep_toolbox::fb_prop(retval, v_in, v_pla, rp, beta, mu);
    return retval;
}

static inline double fb_vel_wrapper(const kep_toolbox::array3D &vin_rel, const kep_toolbox::array3D &vout_rel,
                                    const kep_toolbox::planet::base &pl)
{
    double dV(0);
    kep_toolbox::fb_vel(dV, vin_rel, vout_rel, pl);
    return dV;
}

static inline tuple damon_wrapper(const kep_toolbox::array3D &v1, const kep_toolbox::array3D &v2, double tof)
{
    kep_toolbox::array3D a1, a2;
    double tau, dv;
    kep_toolbox::damon_approx(v1, v2, tof, a1, a2, tau, dv);
    return boost::python::make_tuple(kep_toolbox::array3D(a1), kep_toolbox::array3D(a2), double(tau), double(dv));
}

#define get_constant(arg)                                                                                              \
    static inline double get_##arg()                                                                                   \
    {                                                                                                                  \
        return ASTRO_##arg;                                                                                            \
    }

get_constant(AU);
get_constant(JR);
get_constant(MU_SUN);
get_constant(MU_EARTH);
get_constant(EARTH_VELOCITY);
get_constant(EARTH_J2);
get_constant(EARTH_RADIUS);
get_constant(DEG2RAD);
get_constant(RAD2DEG);
get_constant(DAY2SEC);
get_constant(SEC2DAY);
get_constant(DAY2YEAR);
get_constant(G0);

#define PYKEP_REGISTER_CONVERTER(T, policy)                                                                            \
    {                                                                                                                  \
        boost::python::type_info info = boost::python::type_id<T>();                                                   \
        const boost::python::converter::registration *reg = boost::python::converter::registry::query(info);           \
        if (reg == NULL) {                                                                                             \
            to_tuple_mapping<T>();                                                                                     \
            from_python_sequence<T, policy>();                                                                         \
        } else if ((*reg).m_to_python == NULL) {                                                                       \
            to_tuple_mapping<T>();                                                                                     \
            from_python_sequence<T, policy>();                                                                         \
        }                                                                                                              \
    }

#define PYKEP_EXPOSE_CONSTANT(arg) def("_get_" #arg, &get_##arg);

typedef std::array<double, 8> array8D;
typedef std::array<double, 11> array11D;

BOOST_PYTHON_MODULE(core)
{
    // Disable docstring c++ signature to allow sphinx autodoc to work properly
    docstring_options doc_options;
    doc_options.disable_signatures();

    // Register std converters to python lists if not already registered by some
    // other module
    PYKEP_REGISTER_CONVERTER(std::vector<double>, variable_capacity_policy)
    PYKEP_REGISTER_CONVERTER(kep_toolbox::array3D, fixed_size_policy)
    PYKEP_REGISTER_CONVERTER(kep_toolbox::array6D, fixed_size_policy)
    PYKEP_REGISTER_CONVERTER(kep_toolbox::array7D, fixed_size_policy)
    PYKEP_REGISTER_CONVERTER(array8D, fixed_size_policy)
    PYKEP_REGISTER_CONVERTER(array11D, fixed_size_policy)
    PYKEP_REGISTER_CONVERTER(std::vector<kep_toolbox::array3D>, variable_capacity_policy)
    PYKEP_REGISTER_CONVERTER(std::vector<array8D>, variable_capacity_policy)
    PYKEP_REGISTER_CONVERTER(std::vector<array11D>, variable_capacity_policy)

    // Expose the astrodynamical constants.
    PYKEP_EXPOSE_CONSTANT(AU);
    PYKEP_EXPOSE_CONSTANT(JR);
    PYKEP_EXPOSE_CONSTANT(MU_SUN);
    PYKEP_EXPOSE_CONSTANT(MU_EARTH);
    PYKEP_EXPOSE_CONSTANT(EARTH_VELOCITY);
    PYKEP_EXPOSE_CONSTANT(EARTH_J2);
    PYKEP_EXPOSE_CONSTANT(EARTH_RADIUS);
    PYKEP_EXPOSE_CONSTANT(DEG2RAD);
    PYKEP_EXPOSE_CONSTANT(RAD2DEG);
    PYKEP_EXPOSE_CONSTANT(DAY2SEC);
    PYKEP_EXPOSE_CONSTANT(SEC2DAY);
    PYKEP_EXPOSE_CONSTANT(DAY2YEAR);
    PYKEP_EXPOSE_CONSTANT(G0);

    // Exposing enums
    enum_<kep_toolbox::epoch::type>("_epoch_type", "Defines a julian date type exposing the corresponding c++ enum\n\n"
                                                   "One of\n\n"
                                                   "* pykep.epoch.epoch_type.JD\n"
                                                   "* pykep.epoch.epoch_type.MJD\n"
                                                   "* pykep.epoch.epoch_type.MJD2000\n")
        .value("MJD", kep_toolbox::epoch::MJD)
        .value("MJD2000", kep_toolbox::epoch::MJD2000)
        .value("JD", kep_toolbox::epoch::JD);

    // Epoch class
    class_<kep_toolbox::epoch>("epoch", pykep::epoch_doc().c_str(),
                               init<optional<const double &, kep_toolbox::epoch::type>>())
        .add_property("jd", &kep_toolbox::epoch::jd,
                      "Returns the Julian Date\n\n"
                      "Example::\n\n"
                      "  jd = e.jd")
        .add_property("mjd", &kep_toolbox::epoch::mjd,
                      "Returns the Modified Julian Date\n\n"
                      "Example::\n\n"
                      "  jd = e.mjd")
        .add_property("mjd2000", &kep_toolbox::epoch::mjd2000,
                      "Returns the Modified Julian Date 2000\n\n"
                      "Example::\n\n"
                      "  jd = e.mjd2000")
        .def(repr(self))
        .def_pickle(python_class_pickle_suite<kep_toolbox::epoch>());

    // Epoch constructors helpers
    def("epoch_from_string", &kep_toolbox::epoch_from_string, pykep::epoch_from_string_doc().c_str());
    def("epoch_from_iso_string", &kep_toolbox::epoch_from_iso_string, pykep::epoch_from_iso_string_doc().c_str());

    // Lambert.
    class_<kep_toolbox::lambert_problem>(
        "lambert_problem", "Represents a multiple revolution Lambert's problem",
        init<const kep_toolbox::array3D &, const kep_toolbox::array3D &, const double &, const double &, const int &,
             const int &>(pykep::lambert_problem_doc().c_str(),
                          (arg("r1") = kep_toolbox::array3D{1, 0, 0}, arg("r2") = kep_toolbox::array3D{0, 1, 0},
                           arg("tof") = boost::math::constants::pi<double>() / 2., arg("mu") = 1., arg("cw") = false,
                           arg("max_revs") = 0)))
        .def("get_v1", &kep_toolbox::lambert_problem::get_v1, return_value_policy<copy_const_reference>(),
             "Returns a sequence of vectors containing the velocities at r1 of "
             "all computed solutions to the Lambert's Problem\n\n"
             "Solutions are stored in order 0 rev, 1rev, 1rev, 2rev, 2rev, ...\n\n"
             "Example (extracts v1 for the 0 revs solution)::\n\n"
             "  v10 = l.get_v1()[0]")
        .def("get_v2", &kep_toolbox::lambert_problem::get_v2, return_value_policy<copy_const_reference>(),
             "Returns a sequence of vectors containing the velocities at r2 of "
             "all computed solutions to the Lambert's Problem\n\n"
             "Solutions are stored in order 0 rev, 1rev, 1rev, 2rev, 2rev, ...\n\n"
             "Example (extracts v2 for the 0 revs solution)::\n\n"
             "  v20 = l.get_v2()[0]")
        .def("get_r1", &kep_toolbox::lambert_problem::get_r1, return_value_policy<copy_const_reference>(),
             "Returns a vector containing the r1 defining the Lambert's "
             "Problem\n\n"
             "Example ::\n\n"
             "  r1 = l.get_r1()")
        .def("get_r2", &kep_toolbox::lambert_problem::get_r2, return_value_policy<copy_const_reference>(),
             "Returns a vector containing the r2 defining the Lambert's "
             "Problem\n\n"
             "Example ::\n\n"
             "  r2 = l.get_r2()")
        .def("get_tof", &kep_toolbox::lambert_problem::get_tof, return_value_policy<copy_const_reference>(),
             "Returns the time of flight defining the Lambert's Problem\n\n"
             "Example::\n\n"
             "  t = l.get_tof()")
        .def("get_mu", &kep_toolbox::lambert_problem::get_mu, return_value_policy<copy_const_reference>(),
             "Returns the gravitational parameter defining the Lambert's "
             "Problem\n\n"
             "Example::\n\n"
             "  mu = l.get_mu()")
        .def("get_x", &kep_toolbox::lambert_problem::get_x, return_value_policy<copy_const_reference>(),
             "Returns a sequence containing the x values of all computed "
             "solutions to the Lambert's Problem\n\n"
             "Solutions are stored in order 0 rev, 1rev, 1rev, 2rev, 2rev, ...\n\n"
             "Example (extracts x for the 0 revs solution)::\n\n"
             "  x0 = l.get_x()[0]")
        .def("get_iters", &kep_toolbox::lambert_problem::get_iters, return_value_policy<copy_const_reference>(),
             "Returns a sequence containing the number of iterations employed to "
             "compute each solution to the Lambert's Problem\n\n"
             "Solutions are stored in order 0 rev, 1rev, 1rev, 2rev, 2rev, ...\n\n"
             "Example (extracts the number of iterations employed for the 0 revs "
             "solution)::\n\n"
             "  p0 = l.get_iters()[0]")
        .def("get_Nmax", &kep_toolbox::lambert_problem::get_Nmax,
             "Returns the maximum number of revolutions allowed. The total "
             "number of solution to the Lambert's problem will thus be n_sol = "
             "Nmax*2 + 1\n\n"
             "Example::\n\n"
             "  Nmax = l.get_Nmax()\n"
             "  n_sol = Nmax*2+1")
        .def(repr(self))
        .def_pickle(python_class_pickle_suite<kep_toolbox::lambert_problem>());

    // Lagrangian propagator for keplerian orbits
    def("propagate_lagrangian", &propagate_lagrangian_wrapper, pykep::propagate_lagrangian_doc().c_str(),
        (arg("r0") = kep_toolbox::array3D{1, 0, 0}, arg("v0") = kep_toolbox::array3D{0, 1, 0},
         arg("tof") = boost::math::constants::pi<double>() / 2, arg("mu") = 1));

    // Taylor propagation of inertially constant thrust arcs
    def("propagate_taylor", &propagate_taylor_wrapper, pykep::propagate_taylor_doc().c_str(),
        (arg("r0") = kep_toolbox::array3D{1, 0, 0}, arg("v0") = arg("v0") = kep_toolbox::array3D{0, 1, 0},
         arg("m0") = 100, arg("thrust") = kep_toolbox::array3D{0, 0, 0},
         arg("tof") = boost::math::constants::pi<double>() / 2, arg("mu") = 1, arg("veff") = 1, arg("log10tol") = 1e-15,
         arg("log10rtol") = 1e-15));

    // Taylor propagation of inertially constant thrust arcs with an inertially constant disturbance
    def("propagate_taylor_disturbance", &propagate_taylor_disturbance_wrapper,
        "pykep.propagate_taylor(r,v,m,thrust,disturbance,t,mu,veff,log10tol,log10rtol)\n\n"
        "- r: start position, x,y,z\n"
        "- v: start velocity, vx,vy,vz\n"
        "- m: starting mass\n"
        "- thrust: fixed inertial thrust, cartesian components\n"
        "- disturbance: fixed inertial disturbance force, cartesian components\n"
        "- tof: propagation time\n"
        "- mu: central body gravity constant\n\n"
        "- veff: the product (Isp g0) defining the engine efficiency \n\n"
        "- log10tol: the logarithm of the absolute tolerance passed to taylor propagator \n\n"
        "- log10rtol: the logarithm of the relative tolerance passed to taylor propagator \n\n"
        "Returns a tuple containing r, v, and m the final position, velocity and mass after the propagation.\n\n"
        "Example::\n\n"
        "  r,v,m = propagate_taylor_disturbance([1,0,0],[0,1,0],100,[0,0,0],[1e-2,1e-3,1e-4],pi/2,1,1,-15,-15)",
        (arg("r"), arg("v"), arg("m"), arg("thrust"), arg("disturbance"), arg("tof"), arg("mu"), arg("veff"),
         arg("log10tol") = 1e-15, arg("log10rtol") = 1e-15));
    // Taylor propagation of inertially constant thrust arcs under the J2
    // influence
    def("propagate_taylor_J2", &propagate_taylor_J2_wrapper,
        "pykep.propagate_taylor_J2(r,v,m,thrust,t,mu,veff,J2RG2,log10tol,log10rtol)"
        "\n\n"
        "- r: start position, x,y,z\n"
        "- v: start velocity, vx,vy,vz\n"
        "- m: starting mass\n"
        "- thrust: fixed inertial thrust, ux,uy,uz\n"
        "- tof: propagation time\n"
        "- mu: central body gravity constant\n\n"
        "- veff: the product (Isp g0) defining the engine efficiency \n\n"
        "- J2RG2: the product (J2 RE^2) defining the gravity J2 perturbance \n\n"
        "- log10tol: the logarithm of the absolute tolerance passed to taylor "
        "propagator \n\n"
        "- log10rtol: the logarithm of the relative tolerance passed to taylor "
        "propagator \n\n"
        "Returns a tuple containing r, v, and m the final position, velocity and "
        "mass after the propagation.\n\n"
        "Example::\n\n"
        "  r,v,m = "
        "propagate_taylor([1,0,0],[0,1,0],100,[0,0,0],pi/2,1,1,1e-3,-15,-15)",
        (arg("r"), arg("v"), arg("m"), arg("thrust"), arg("tof"), arg("mu"), arg("veff"), arg("log10tol") = 1e-15,
         arg("log10rtol") = 1e-15));
    // Taylor propagation of inertially constant thrust arcs (using the
    // generalized sundmann variable)
    def("propagate_taylor_s", &propagate_taylor_s_wrapper,
        "pykep.propagate_taylor_s(r,v,m,u,s,mu,veff,c=1,alpha,log10tol,log10rtol]"
        ")\n\n"
        "- r: start position, x,y,z\n"
        "- v: start velocity, vx,vy,vz\n"
        "- m: starting mass\n"
        "- u: fixed inertial thrust, ux,uy,uz\n"
        "- s: propagation pseudo-time\n"
        "- mu: central body gravity constant\n\n"
        "- veff: the product (Isp g0) defining the engine efficiency \n\n"
        "- c: constant coefficient in the generalized Sundmann transform \n\n"
        "- alpha: r exponent in the generalized Sundmann transform \n\n"
        "- log10tol: the logarithm of the absolute tolerance passed to taylor "
        "propagator \n\n"
        "- log10rtol: the logarithm of the relative tolerance passed to taylor "
        "propagator \n\n"
        "Returns a tuple containing r, v, m and t the final position, velocity, "
        "mass and time after the propagation pseudo-time s\n\n"
        "Example::\n\n"
        "  r,v,m,t = "
        "propagate_taylor_s([1,0,0],[0,1,0],100,[0,0,0],pi/"
        "2,1.0,1.0,1.0,1.0,-10,-10)");

    // Fly-by helper functions
    def("fb_con", &fb_con_wrapper,
        "pykep.fb_con(vin,vout,pl)\n\n"
        "- vin: cartesian coordinates of the relative hyperbolic velocity before "
        "the fly-by\n"
        "- vout: vout, cartesian coordinates of the relative hyperbolic velocity "
        "after the fly-by\n"
        "- pl: fly-by planet\n\n"
        "Returns a tuple containing (eq, ineq). \n"
        "  eq represents the violation of the equality constraint norm(vin)**2 = "
        "norm(vout)**2.\n"
        "  ineq represents the violation of the inequality constraint on the "
        "hyperbola asymptote maximum deflection\n\n"
        "Example::\n\n"
        "  v2_eq, delta_ineq = fb_con(vin, vout, planet_ss('earth'))\n"
        "  v2_eq = v2_eq / EARTH_VELOCITY**2\n");
    def("fb_prop", &fb_prop_wrapper,
        "pykep.fb_prop(v,v_pla,rp,beta,mu)\n\n"
        "- v: spacecarft velocity before the encounter (cartesian, absolute)\n"
        "- v-pla: planet inertial velocity at encounter (cartesian, absolute)\n"
        "- rp: fly-by radius\n"
        "- beta: fly-by plane orientation\n"
        "- mu: planet gravitational constant\n\n"
        "Returns the cartesian components of the spacecarft velocity after the "
        "encounter. \n"
        "Example::\n\n"
        "  vout = fb_prop([1,0,0],[0,1,0],2,3.1415/2,1)\n");
    def("fb_vel", &fb_vel_wrapper,
        "pykep.fb_vel(vin,vout,pl)\n\n"
        "- vin: cartesian coordinates of the relative hyperbolic velocity before "
        "the fly-by\n"
        "- vout: vout, cartesian coordinates of the relative hyperbolic velocity "
        "after the fly-by\n"
        "- pl: fly-by planet\n\n"
        "Returns a number dV. \n"
        "  dV represents the norm of the delta-V needed to make a given fly-by "
        "possible. dV is necessary to fix the magnitude and the direction of "
        "vout.\n\n"
        "Example::\n\n"
        "  dV = fb_vel(vin, vout, planet_ss('earth'))\n");

    // Basic Astrodynamics
    def("closest_distance", &closest_distance_wrapper,
        "pykep.closest_distance(r1,v1,r2,v2,mu = 1.0)\n\n"
        "- r1: initial position (cartesian)\n"
        "- v1: initial velocity (cartesian)\n"
        "- r2: final position (cartesian)\n"
        "- v2: final velocity (cartesian)\n"
        "- mu: planet gravitational constant\n\n"
        "Returns a tuple containing the closest distance along a keplerian orbit "
        "defined by r1,v1,r2,v2 (it assumes the orbit actually exists) and the "
        "apoapsis radius of the resulting orbit. \n"
        "Example::\n\n"
        "  d,ra = closest_distance([1,0,0],[0,1,0],[0,1,0],[-1,0,0],1.0)\n",
        (arg("r1"), arg("v1"), arg("r2"), arg("v2"), arg("mu") = 1.0));

    def("barker", &kep_toolbox::barker,
        "pykep.barker(r1,r2,mu = 1.0)\n\n"
        "- r1: initial position (cartesian)\n"
        "- r2: final position (cartesian)\n"
        "- mu: gravity parameter\n\n"
        "Returns the time of flight as evaluated by the Barker equation. \n"
        "Example:: \n\n"
        "  t = barker([1,0,0],[0,1,0],1.0)",
        (arg("r1"), arg("r2"), arg("mu") = 1.0));

    def("ic2par", &ic2par_wrapper,
        "pykep.ic2par(r,v,mu = 1.0)\n\n"
        "- r: position (cartesian)\n"
        "- v: velocity (cartesian)\n"
        "- mu: gravity parameter\n\n"
        "Returns the osculating keplerian elements a,e,i,W,w,E\n"
        "E is the eccentric anomaly for e<1, the Gudermannian for e>1\n"
        "a is the semi-major axis always a positive quantity.\n"
        "NOTE: The routine is singular when the elements are not defined.\n"
        "Example:: \n\n"
        "  el = ic2par([1,0,0],[0,1,0],1.0)",
        (arg("r"), arg("v"), arg("mu") = 1.0));

    def("par2ic", &par2ic_wrapper,
        "pykep.par2ic(E,mu = 1.0)\n\n"
        "- E: osculating keplerian elements a,e,i,W,w,E ( l, ND, rad, rad, rad, rad)\n"
        "- mu: gravity parameter (l^3/s^2)\n\n"
        "Returns cartesian elements from Keplerian elements\n"
        "E is the eccentric anomaly for e<1, the Gudermannian for e>1\n"
        "a is the semi-major axis always a positive quantity.\n"
        "Example:: \n\n"
        "  r, v = pk.par2ic([1,0.3,0.1,0.1,0.2,0.2], 1)",
        (arg("E"), arg("mu") = 1.0));

    def("ic2eq", &ic2eq_wrapper,
        "pykep.ic2eq(r,v,mu = 1.0, retrogade = False)\n\n"
        "- r: position (cartesian)\n"
        "- v: velocity (cartesian)\n"
        "- mu: gravity parameter\n\n"
        "- retrogade: uses the retrograde parameters. Default value is False.\n\n"
        "Returns the modified equinoctial elements a(1-e^2),f,g,h,k,L\n"
        "L is the true mean longitude\n"
        "Example:: \n\n"
        "  E = ic2eq(r = [1,0,0], v = [0,1,0], mu =1.0)",
        (arg("r"), arg("v"), arg("mu") = 1.0, arg("retrograde") = false));

    def("eq2ic", &eq2ic_wrapper,
        "pykep.eq2ic(EQ,mu = 1.0, retrograde = False)\n\n"
        "- EQ: modified equinoctial elements a(1-e^2),f,g,h,k,L\n"
        "- mu: gravity parameter (l^3/s^2)\n\n"
        "Returns cartesian elements from Keplerian elements\n"
        "L is the true longitude\n"
        "Example:: \n\n"
        "  r, v = pk.eq2ic(eq = [1,0.3,0.1,0.1,0.2,0.2], mu = 1, retrograde = False)",
        (arg("eq"), arg("mu") = 1.0, arg("retrograde") = false));

    def("eq2par", &eq2par_wrapper,
        "pykep.eq2par(EQ, retrograde = False)\n\n"
        "- EQ: modified equinoctial elements a(1-e^2),f,g,h,k,L\n"
        "- retrogade: uses the retrograde parameters. Default value is False.\n\n"
        "Returns the osculating Keplerian elements a,e,i,W,w,E \n"
        "E is the eccentric anomaly for e<1, the Gudermannian for e>1\n"
        "a is the semi-major axis, always a positive quantity.\n"
        "L is the true longitude\n"
        "Example:: \n\n"
        "  E = pk.eq2par(eq = [1,0.3,0.1,0.1,0.2,0.2], retrograde = False)",
        (arg("eq"), arg("retrograde") = false));

    def("par2eq", &par2eq_wrapper,
        "pykep.par2eq(E, retrograde = False)\n\n"
        "- E: osculating keplerian elements a,e,i,W,w,E ( l, ND, rad, rad, rad, rad)\n"
        "- retrogade: uses the retrograde parameters. Default value is False.\n\n"
        "Returns the modified equinoctial elements a(1-e^2),f,g,h,k,L\n"
        "L is the true mean longitude\n"
        "Example:: \n\n"
        "  E = pk.par2eq(E = [1.0,0.1,0.2,0.3,0.4,0.5], retrograde = False)",
        (arg("E"), arg("retrograde") = false));

    def("damon", &damon_wrapper,
        "pykep.damon(v1,v2,tof)\n\n"
        "- v1: starting velocity relative to the departure body. This is\n"
        "      computed from the Lambert solution linking initial and\n"
        "      final position in tof\n"
        "- v2: ending velocity relative to the arrival body. This is\n"
        "      computed from the Lambert solution linking initial and\n"
        "      final position in tof\n"
        "- tof: time of flight\n\n"
        "Returns:\n\n"
        "- a1: acceleration vector in the first part of the transfer\n"
        "- a2: acceleration vector in the second part of the transfer\n"
        "- tau: duration of the first part of the transfer\n"
        "- dv: total estimated DV\n\n"
        "This function uses the model developed by Damon Landau (JPL)\n"
        "during GTOC7 to compute an approximation to the low-thrust transfer\n\n"
        "Example:: \n\n"
        " a1, a2, tau, dv = pykep.damon(v1,v2,tof)",
        (arg("v1"), arg("v2"), arg("tof")));

    def("max_start_mass", &kep_toolbox::max_start_mass,
        "pykep.max_start_mass(a, dv, T_max, Isp)\n\n"
        "- a: acceleration magnitude as computed from Damon's model\n"
        "- dv: dv magnitude as computed from Damon's model\n"
        "- T_max: maximum thrust of the spacecarft NEP propulsion system\n"
        "- Isp: specific impulse of the spacecarft NEP propulsion system\n\n"
        "Returns the estimated maximum mass at leg beginning\n"
        "This function estimates the maximum\n"
        "mass a NEP spacececraft can have in order for a given\n"
        "arc to be convertable into a valid low-thrust transfer\n\n"
        "Example:: \n\n"
        " a,_,_,dv = pykep.damon(v1,v2,tof)\n"
        " m = pykep.max_start_mass(norm(a), dv, T_max, Isp)");

    // For the three_impulses_approximation we need to distinguish the overloaded
    // cases. We thus introduce new function pointers
    double (*three_imp)(double, double, double, double, double, double, double, double, double)
        = &kep_toolbox::three_impulses_approx;
    double (*three_imp_2_pl)(const kep_toolbox::planet::base &, const kep_toolbox::planet::base &)
        = &kep_toolbox::three_impulses_approx;
    double (*three_imp_2_pl_2_ep)(const kep_toolbox::planet::base &, const kep_toolbox::planet::base &,
                                  const kep_toolbox::epoch &, const kep_toolbox::epoch &)
        = &kep_toolbox::three_impulses_approx;

    def("_three_impulses_approx", three_imp);
    def("_three_impulses_approx", three_imp_2_pl);
    def("_three_impulses_approx", three_imp_2_pl_2_ep);
}
