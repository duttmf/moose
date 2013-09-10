#Run with 4 procs

[Mesh]
  file = cube.e
[]

[Variables]
  active = 'x_disp y_disp z_disp'

  [./x_disp]
    order = FIRST
    family = LAGRANGE
  [../]

  [./y_disp]
    order = FIRST
    family = LAGRANGE
  [../]

  [./z_disp]
    order = FIRST
    family = LAGRANGE
  [../]
[]

[SolidMechanics]
  [./solid]
    disp_x = x_disp
    disp_y = y_disp
    disp_z = z_disp
  [../]
[]

[BCs]
  active = 'y_force left bottom'

  [./y_force]
    type = NeumannBC
    variable = y_disp
    boundary = 2
    value = -1.0
  [../]

  [./left]
    type = DirichletBC
    variable = x_disp
    boundary = 3
    value = 0.0
  [../]

  [./bottom]
    type = DirichletBC
    variable = y_disp
    boundary = 1
    value = 0.0
  [../]

[]

[Materials]
  active = 'constant'

  [./constant]
    type = LinearIsotropicMaterial
    block = 1
    youngs_modulus = 1e6
    poissons_ratio = .3
    disp_x = x_disp
    disp_y = y_disp
    disp_z = z_disp
  [../]
[]

[Executioner]
  type = Steady #Transient

  solve_type = PJFNK
  petsc_options = '-ksp_monitor'

  l_max_its = 100

#  start_time = 0.0
#  num_steps = 4
#  dt = 0.000005
[]

[Output]
  file_base = out
  interval = 1
  output_initial = true
  exodus = true
#  gmv = true
#  tecplot = true
  perf_log = true
[]


