# Map OBJ Material 

This utility maps materials from a source `.obj` to a target `.obj`. Only 
vertices `v`, faces `f`, and `usemtl` lines are read and written.  

## Installation  

### Conda Linux  

1. Create a conda environment with the necessary preqrequisites via the 
`environment.yml` file:  

    ```bash
    conda env create -f environment.yml
    conda activate map-obj-mtl
    ```

2. Build and compile the program:  

    ```bash
    cmake -S . -B ./build-linux
    make -C ./build-linux
    ```

3. Run the program:  

    ```bash
    ./build-linux/map_obj_mtl ./data/Ifc4_SampleHouse.obj ./data/Ifc4_SampleHouse_0.500000_0.001000_-1.000000.obj ./data/Ifc4_SampleHouse_materials.obj
    ```

4. Have the output reference the correct material file. Assuming the above 
command was executed, this can be done in two ways:  
    
    1. Add the line `mtllib Ifc4_SampleHouse.mtl` to 
    the start of the output file  
    2. Copy or rename the source material file by running  
    ```bash
    cp ./data/Ifc4_SampleHouse.mtl ./data/Ifc4_SampleHouse_materials.mtl
    ```

## Arguments  

| Name     | Type          | Description         | Required | Default   |
|----------|---------------|---------------------|----------|-----------|
| `source` | `std::string` | Source OBJ filename | `true`   | -         |
| `target` | `std::string` | Target OBJ filename | `true`   | -         |
| `out`    | `std::string` | Output filename     | `false`  | `out.obj` |

