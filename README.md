# Map OBJ Material 

This utility maps materials from a source `.obj` to a target `.obj`. Only 
vertices `v`, faces `f`, `usemtl`, and `mtllib` lines are read and written.  

| Source | Target | Output |
| - | - | - |
| ![Tembusu L12](./img/Tembusu%20L12.png) | ![Tembusu L12 wrap](./img/Tembusu%20L12%20wrap.png) | ![Tembusu L12 materials](./img/Tembusu%20L12%20materials.png) |
| ![KPF Robinson Tower](./img/KPF%20Robinson%20Tower.png) | ![KPF Robinson Tower wrap](./img/KPF%20Robinson%20Tower%20wrap.png) | ![KPF Robinson Tower materials](./img/KPF%20Robinson%20Tower%20materials.png) |

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
    cmake --build ./build-linux
    ```

3. Run the program:  

    ```bash
    ./build-linux/map_obj_mtl ./data/Tembusu\ L12.obj ./data/Tembusu\ L12\ wrap.obj ./data/Tembusu\ L12\ materials.obj
    ```

4. Have the output reference the correct material file. Assuming the above 
command was executed, the output file should already correctly reference the
existing material file. Otherwise, make a copy of the source material file and
rename it:  
     
    ```bash
    cp ./data/Tembusu\ L12.mtl ./data/Tembusu\ L12\ materials.mtl
    ```

## Arguments  

| Name     | Type          | Description         | Required | Default   |
|----------|---------------|---------------------|----------|-----------|
| `source` | `std::string` | Source OBJ filename | `true`   | -         |
| `target` | `std::string` | Target OBJ filename | `true`   | -         |
| `out`    | `std::string` | Output filename     | `false`  | `out.obj` |

