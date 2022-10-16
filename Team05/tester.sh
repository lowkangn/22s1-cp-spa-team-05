 #!/bin/bash

 # config
config=Debug
while getopts 'r' flag
do
    case "${flag}" in
        r) config=Release
    esac
done

# ==================== test config ====================
# autotester path
readonly path_to_autotester=./Code05/out/build/x64-${config}/src/autotester/autotester.exe

# 0. sample
readonly sample_path=./Tests05/

# 1. iteration 1
readonly iteration1=./Tests05/iteration1/
readonly iteration1_output_folder=./Code05/testOutputs/iteration1/

# 2. iteration 1 bug fix
readonly milestoneBugs_path=./Tests05/iteration1/conditionsInWhile/

# 2. iteration 2
readonly iteration2=./Tests05/iteration2/
readonly iteration2_output_folder=./Code05/testOutputs/iteration2/

readonly invalid_q_path=invalidQuery/
readonly invalid_s_path=invalidSource/
readonly no_constr_path=noConstraintsQuery/
readonly one_constr_path=oneConstraintQuery/
readonly two_constr_path=twoConstraintsQuery/
readonly boolean_return_values_path=booleanReturnValues/


# ==================== run tests ====================
if [ ! -d $iteration1_output_folder ]; then
	echo "Making output folder: "${iteration1_output_folder}
	mkdir -p $iteration1_output_folder
fi

if [ ! -d $iteration2_output_folder ]; then
	echo "Making output folder: "${iteration2_output_folder}
	mkdir -p $iteration2_output_folder
fi

echo "Using autotester at: "${path_to_autotester}




echo "starting boolean return value test cases"
# no constraints
${path_to_autotester} ${iteration2}${boolean_return_values_path}noConstraints_source.txt ${iteration2}${boolean_return_values_path}noConstraints_queries.txt ${iteration2_output_folder}booleanReturnValuesNoConstraintsOut.xml

# all false
${path_to_autotester} ${iteration2}${boolean_return_values_path}noneSatisfyingReturnFalse_source.txt ${iteration2}${boolean_return_values_path}noneSatisfyingReturnFalse_queries.txt ${iteration2_output_folder}booleanReturnValuesNoneSatisfyingOut.xml

# all true
${path_to_autotester} ${iteration2}${boolean_return_values_path}someSatistyingReturnTrue_source.txt ${iteration2}${boolean_return_values_path}someSatistyingReturnTrue_queries.txt ${iteration2_output_folder}booleanReturnValuesSomeSatisfyingOut.xml


# all true

