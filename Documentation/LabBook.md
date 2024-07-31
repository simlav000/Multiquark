# Lab Book

## Coordinates and Momentum 
The momentum of a particle is the product of its mass and its velocity
$$ p = m\cdot v = (p_x, p_y, p_z)$$
In spherical coordinates centered at the collision point
$p =(|p|, \theta, \phi)$, where $\theta$ is the polar angle, and $\phi$
is the azimuthal angle. Since most detectors are cylindrical, however, 
a new magnitude known as **pseudorapidity** is used
$$\eta = -\ln\left(\tan\left(\frac{\theta}{2}\right)\right) $$
with $\eta \in (-\infty, \infty)$. In hardron collider physics, $\eta$ is 
preferred over $\theta$ because, loosely speaking, particle production 
is constant as a function of pseudorapidity.

Besides, tranvserse momentum ($p_T$), is also preferred and it is 
calculated from tranvseral energy, delivered upon the calorimeters.

So, in particle physics, linear momentum is presented by 
$$p = (p_T, \eta, \phi)$$
To obtain cartesian momenta $(p_x, p_y, p_z)$, with the $z$-axis 
corresponding to the axis of the beam, the following conversions are used:
 - $p_x = p_T\cos(\phi)$
 - $p_y = p_T\sin(\phi)$
 - $p_z = p_T\sinh(\eta)$

With the following results:
$$
\begin{aligned}
 |p| &= \sqrt{p_x^2 + p_y^2 + p_z^2} \\
     &= \sqrt{p_T^2\cos^2(\phi) + p_T^2\sin^2(\phi) + p_T^2\sinh^2(\eta)} \\
     &= p_T\sqrt{1 + \sinh^2(\eta)} \\
     &= p_T\cosh(\eta)
\end{aligned}
$$
by the identity $\cosh^2(x) - \sinh^2(x) = 1$. And of course we also have
$$p_T = \sqrt{p_x^2 + p_y^2}$$

## Good Runs Lists
In the ATLAS experiment, Good Runs Lists (GRLs) are lists
characterizing the overall quality of data, related to the
functioning of detector components at the time. These runs 
are composed of luminosity blocks (LBs), which are spans of 
time lasting roughly a minute. A good run is one in which
all data quality flags are green, which occurs when 
conditions within the detector are stable. 

The `GRLSelectorAlg` is an algorithm that runs a GRL against the data. It is enable by adding the following lines to the `config.yaml` in the `data/` directory:
```yaml
EventCleaning:
    runEventCleaning: True
```
and then running the analysis job with the command:
`ATestRun_eljob.py --config-path=/relative/path/to/config.yaml --submission-dir=submitDir`

## Determining lifetime of particles
The mean lifetime of a particle can be determined using
$$ d = ct\gamma\beta$$
where $\beta = v/c$ and $\gamma = 1/\sqrt{(1-\beta^2)}$, and $d$ is the distance
between the primary and secondary vertices. Using the relativistic
momentum $p = \gamma m v$, we can rearrange this to obtain:
$$ t = \frac{dm}{pc} $$
Thus we can determine the lifetime of a particle using nothing but the distance
between primary and secondary vertices, the mass of the particle (as given by
the PDG), and it's 3D momentum, which is easily accessible using: 
  - `SG::AuxElement::ConstAccessor<float> getPx( "px" );`
  - `SG::AuxElement::ConstAccessor<float> getPy( "py" );`
  - `SG::AuxElement::ConstAccessor<float> getPz( "pz" );`
  - and the primary and secondary vertices' `x()`, `y()` and `z()` methods to 
  determine $d$.

The first  four bullets show methods outlining the way to obtain information 
about secondary vertices within an AnalysisBase project. Certain tools 
such as `InDetV0Finder` have been used to process the ATLAS data and make 
such pieces of information easily accessible.

## Fitting Particle Lifetime Distribution
The lifetime distribution of particles can be fit using an exponential 
model of the form
$$f(t) = C_Ae^{t/\tau_A} + C_Be^{t/\tau_B} + C_0$$
Where $C_A$ and $\tau_A$ model particle A's lifetime, and $C_0$, $C_A$ and 
$\tau_B$ models the background lifetime.

## $K^0_s$ and $\Lambda^0$ Invariant Mass Plots
Each secondary vertex is assigned a kaon and lambda mass. These
two quantities can be plotted to obtain invariant mass distributions
which contain a distinct signal (seen as a spike) and a background. 
To isolate this signal, certain cuts can be performed to properly 
identify the kaons and lambdas.

<!--TODO: Use correct image -->
![Kaon invariant mass histogram](../Histograms/20240618/KMass_Before_and_After_Cuts.png)

## Bump Hunt Technique
Given a $K^0_sK^0_s$ invariant mass histogram, for example, the high-energy
portion of the spectrum can be searched for resonant states using the bump-hunt
technique. The goal is to search for bins which are suspiciously large compared
to the background. Each bin is a candidate in this search. The idea is to make
a copy of the original histogram with a single bin removed, doing this
iteratively for each bin (one at a time). The background model is then fit to
this resulting histogram, and the difference $\Delta$  between the missing bin and the
background model is computed. The standard deviation of this difference is
given by $\Delta/\sqrt{n}$ where $n$ is the count of the removed bin. A plot of each
standard deviation is created, showing the significance of the $\Delta$ measurement. 
A significance of $3\sigma$ is considered possible observation while those with
$5\sigma$ are considered discovery.

The background fit of the form 
$$ y(x) = e^{-(ax^2+bx+c)} $$
is performed yielding best-fit
parameters, each with their own uncertainties. One 
way to propagate the uncertainty of such an 
expression is by using a toy-monte carlo. By simply
defining gaussian distributions with means equal to
the nominal value of the best-fit-parameters, and 
the standard deviations equal to their errors, you 
can randomly sample fit parameters, and repeatedly
evaluate the fit function with these parameters at
the desired point. This yields a distribution whose
mean you can call the value of the function at that 
point, and whose standard error on the mean is the 
uncertainty of the measurement. If 
you believe the errors on the best-fit parameters 
are not representative of the actual uncertainties, 
you can widen them by some percentage of the mean.
Doing so seems like a tricky art.

## Look-Elsewhere Effect
Since we are iteratively searching the space for a 
signal exceeding $5\sigma$, we ought to address this
important effect. Consider the probability of
knowing a $5\sigma$ signal should exist at 6500 MeV
and finding a $5\sigma$ signal  exactly there. 
Now compare that to the probability of finding a 
$5\sigma$ signal anywhere. The latter is much more 
likely, meaning such a finding is much less 
significant than a discovery made when searching a
specific location. To address this, I'm not sure
what I have to do????

## Dataset Info
clean1.root + clean2.root -> Trace near 1710 MeV

