//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "StressDivergenceRSphericalTensors.h"
#include "ElasticityTensorTools.h"
#include "FEProblem.h"
#include "MooseMesh.h"

registerMooseObject("TensorMechanicsApp", StressDivergenceRSphericalTensors);

defineLegacyParams(StressDivergenceRSphericalTensors);

InputParameters
StressDivergenceRSphericalTensors::validParams()
{
  InputParameters params = StressDivergenceTensors::validParams();
  params.addClassDescription(
      "Calculate stress divergence for a spherically symmetric 1D problem in polar coordinates.");
  params.set<unsigned int>("component") = 0;
  params.set<bool>("use_displaced_mesh") = true;
  return params;
}

StressDivergenceRSphericalTensors::StressDivergenceRSphericalTensors(
    const InputParameters & parameters)
  : StressDivergenceTensors(parameters)
{
  if (_component != 0)
    mooseError("Invalid component for this 1D RSpherical problem.");
}

void
StressDivergenceRSphericalTensors::initialSetup()
{
  if (getBlockCoordSystem() != Moose::COORD_RSPHERICAL)
    mooseError("The coordinate system in the Problem block must be set to RSPHERICAL for 1D "
               "spherically symmetric geometries.");
}

Real
StressDivergenceRSphericalTensors::computeQpResidual()
{
  return _grad_test[_i][_qp](0) * _stress[_qp](0, 0) +               // stress_{rr} part 1
         +(_test[_i][_qp] / _q_point[_qp](0)) * _stress[_qp](1, 1) + // stress_{\theta \theta}
         +(_test[_i][_qp] / _q_point[_qp](0)) * _stress[_qp](2, 2);  // stress_{\phi \phi}
}

Real
StressDivergenceRSphericalTensors::computeQpJacobian()
{
  return calculateJacobian(_component, _component);
}

Real
StressDivergenceRSphericalTensors::computeQpOffDiagJacobian(unsigned int jvar)
{
  for (unsigned int i = 0; i < _ndisp; ++i)
    if (jvar == _disp_var[i])
      return calculateJacobian(_component, i);

  if (_temp_coupled && jvar == _temp_var)
    return 0.0;

  return 0.0;
}

Real
StressDivergenceRSphericalTensors::calculateJacobian(unsigned int ivar, unsigned int jvar)
{
  RealGradient test_r, phi_r;

  mooseAssert(ivar == 0 && jvar == 0,
              "Invalid component in Jacobian Calculation"); // Only nonzero case for a 1D simulation

  if (ivar == 0) // Case grad_test for r, requires contributions from stress_{rr}, stress_{\theta
                 // \theta}, and stress_{\phi \phi}
  {
    test_r(0) = _grad_test[_i][_qp](0);
    test_r(1) = _test[_i][_qp] / _q_point[_qp](0);
    test_r(2) = _test[_i][_qp] / _q_point[_qp](0);
  }

  if (jvar == 0)
  {
    phi_r(0) = _grad_phi[_j][_qp](0);
    phi_r(1) = _phi[_j][_qp] / _q_point[_qp](0);
    phi_r(2) = _phi[_j][_qp] / _q_point[_qp](0);
  }

  return ElasticityTensorTools::elasticJacobian(_Jacobian_mult[_qp], ivar, jvar, test_r, phi_r);
}
