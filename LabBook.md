# 12/06/24
- For the past few day's I've been looking at this code:
```C++
  // For multiquark mass method 1 below: Count number of kaons to know when there are only 2 
  if (only2K){
    for ( auto secvtx: *v0_sc ) {
        float px_sVtx  =  pxacc( *secvtx ) ;  //MeV
        float py_sVtx = pyacc( *secvtx );  //MeV
        
        float magPVtx =  sqrt(py_sVtx*py_sVtx + px_sVtx*px_sVtx); // |Pvertex| = sqrt(px^2 + py^2)
        
        float deltaX = (secvtx -> x() - primaryVertex -> x()); // deltaX = x_sv - x_pv
        float deltaY = (secvtx -> y() - primaryVertex -> y());
        float deltaZ = (secvtx -> z() - primaryVertex -> z());

        float magdeltaR = sqrt(deltaX*deltaX +deltaY*deltaY); // Distance |R| = sqrt(deltaX^2 + deltaY^2)      
        float myCosTheta = (px_sVtx*deltaX + py_sVtx*deltaY)/(magPVtx*magdeltaR); // From dot product
        
        float pt =  sqrt( px_sVtx*px_sVtx + py_sVtx*py_sVtx); 
        float cosTheta = myCosTheta;
        float mass    = massacc( *secvtx );
        float massL    = massaccL( *secvtx );
        float delR = sqrt( deltaX*deltaX +deltaY*deltaY + deltaZ * deltaZ ); // 3D distance
        
        // Check if vertex passes Kaon cuts
        if (cosTheta > COS_THETA_CUT && mass > K_MASS_LOW && mass < K_MASS_HIGH && massL > L_MASS_LOW2 && delR > K_3D_DIST && pt > K_PT){ 
            Kcounter++;
	}
        if (cosTheta > COS_THETA_CUT && massL > L_MASS_LOW && massL < L_MASS_HIGH && mass > K_MASS_LOW2 && mass < K_MASS_HIGH2 && delR > L_3D_DIST && pt > L_PT){
            Lcounter++;
	}
	//std::cout << "Kcounter inside loop = " << Kcounter << std::endl;
	//std::cout << "Lcounter inside loop = " << Lcounter << std::endl;
	//std::cout << "Vtx = " << secvtx << std::endl;
    }
  }
```

- From what I understand, we determine if a secondary vertex is a Kshort/Lambda
based off of it falling withing the bounds determined by the cuts. This seems
sensible but perhaps flawed. 
- According to the ATLASSoftwareDocs, vertices land within an enum of various
VertexTypes, those being:
```
enum VertexTypes = {
}
```

- In my testing, all MinBias run2 data has `vertexType == 5` but maybe run 3
data actually flags Kshorts and Lambdas as such. This would give me a great way
to test if the above method is correct by performing both and seeing if their
results are in agreement.
