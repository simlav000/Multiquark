# Lab Book

## Determining lifetime of particles
The mean lifetime of a particle can be determined using
$$ d = ct\gamma\beta$$
where $\beta = v/c$ and $\gamma = 1/\sqrt{(1-\beta^2)}$, and $d$ is the distance
between the primary and secondary vertices. Using the relativistic
momentum $p = \gamma m v$, we can rearrange this to obtain:
$$ t = \frac{d}{c\gamma\beta} = \frac{dm}{pc} $$
Thus we can determine the lifetime of a particle using nothing but the distance
between primary and secondary vertices, the mass of the particle, and it's 3D 
momentum, which are all easily accessible using: 
  - `SG::AuxElement::ConstAccessor<float> getKMass("Kshort_mass");`
  - `SG::AuxElement::ConstAccessor<float> getPx( "px" );`
  - `SG::AuxElement::ConstAccessor<float> getPy( "py" );`
  - `SG::AuxElement::ConstAccessor<float> getPz( "pz" );`
  - and the primary and secondary vertices' `x()`, `y()` and `z()` methods to 
  determine $d$.

