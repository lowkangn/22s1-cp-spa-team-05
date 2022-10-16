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
readonly multipleReturn=multipleReturn/
readonly no_constr_path=noConstraintsQuery/
readonly one_constr_path=oneConstraintQuery/
readonly two_constr_path=twoConstraintsQuery/
readonly boolean_return_values_path=booleanReturnValues/


# ==================== run tests ====================
echo "Using autotester at: "${path_to_autotester}

if [ ! -d $iteration1_output_folder ]; then
	echo "Making output folder: "${iteration1_output_folder}
	mkdir -p $iteration1_output_folder
fi

if [ ! -d $iteration2_output_folder ]; then
	echo "Making output folder: "${iteration2_output_folder}
	mkdir -p $iteration2_output_folder
fi

echo "Using autotester at: "${path_to_autotester}

echo "Starting invalid query test cases"
for i in Declaration Follows Modifies Parent Select Uses Pattern
do	
    ${path_to_autotester} ${iteration1}${invalid_q_path}invalid$i\_source.txt ${iteration1}${invalid_q_path}invalid$i\_queries.txt ${iteration1_output_folder}invalidQueryOut$i.xml
done

for i in Calls
do	
    ${path_to_autotester} ${iteration2}${invalid_q_path}invalid$i\_source.txt ${iteration2}${invalid_q_path}invalid$i\_queries.txt ${iteration2_output_folder}invalidQueryOut$i.xml
done

echo "Starting invalid source test cases"
for i in {1..19}
do	
        ${path_to_autotester} ${iteration1}${invalid_s_path}invalidSource$i\_source.txt ${iteration1}${invalid_s_path}invalidSource$i\_queries.txt ${iteration1_output_folder}invalidSourceOut$i.xml
done

for i in {1..4}
do	
        ${path_to_autotester} ${iteration2}${invalid_s_path}invalidSource$i\_source.txt ${iteration2}${invalid_s_path}invalidSource$i\_queries.txt ${iteration2_output_folder}invalidSourceOut$i.xml
done

echo "Starting milestone test cases"
for i in {1..3}
do	
    ${path_to_autotester} ${milestoneBugs_path}whileStatementTest$i\_source.txt ${milestoneBugs_path}whileStatementTest$i\_queries.txt ${iteration1_output_folder}whileStatementTest$i.xml
done

echo "Starting no constraints query test cases"
${path_to_autotester} ${iteration1}${no_constr_path}noConstraints_source.txt ${iteration1}${no_constr_path}noConstraints_queries.txt ${iteration1_output_folder}outNoConstraints.xml
${path_to_autotester} ${iteration2}${no_constr_path}noConstraints_source.txt ${iteration2}${no_constr_path}noConstraints_queries.txt ${iteration2_output_folder}outNoConstraints.xml

echo "Starting one constraint query test cases"
for i in follows modifies parent pattern uses
do
    ${path_to_autotester} ${iteration1}${one_constr_path}${i}Only_source.txt ${iteration1}${one_constr_path}${i}Only_queries.txt ${iteration1_output_folder}${i}onlyOut.xml
done

for i in calls modifies uses
do
    ${path_to_autotester} ${iteration2}${one_constr_path}${i}Only_source.txt ${iteration2}${one_constr_path}${i}Only_queries.txt ${iteration2_output_folder}${i}onlyOut.xml
done

echo "Starting two constraint query test cases"
for i in parent uses follows modifies
do
    ${path_to_autotester} ${iteration1}${two_constr_path}${i}AndPattern_source.txt ${iteration1}${two_constr_path}${i}AndPattern_queries.txt ${iteration1_output_folder}${i}AndPatternOut.xml
done

echo "starting boolean return value test cases"
# no constraints
${path_to_autotester} ${iteration2}${boolean_return_values_path}noConstraints_source.txt ${iteration2}${boolean_return_values_path}noConstraints_queries.txt ${iteration2_output_folder}booleanReturnValuesNoConstraintsOut.xml

# all false
${path_to_autotester} ${iteration2}${boolean_return_values_path}noneSatisfyingReturnFalse_source.txt ${iteration2}${boolean_return_values_path}noneSatisfyingReturnFalse_queries.txt ${iteration2_output_folder}booleanReturnValuesNoneSatisfyingOut.xml

# all true
${path_to_autotester} ${iteration2}${boolean_return_values_path}someSatistyingReturnTrue_source.txt ${iteration2}${boolean_return_values_path}someSatistyingReturnTrue_queries.txt ${iteration2_output_folder}booleanReturnValuesSomeSatisfyingOut.xml

echo "Starting multiple return query test cases"
for i in multipleReturn
do
    ${path_to_autotester} ${iteration2}${multipleReturn}multipleReturn_source.txt ${iteration2}${multipleReturn}multipleReturn_queries.txt ${iteration2_output_folder}multipleReturn.xml
done

# For random testing, modify Sample_queries.txt and Sample_source.txt
echo "starting local sample test cases"
${path_to_autotester} ${sample_path}Sample_source.txt ${sample_path}Sample_queries.txt ${iteration1_output_folder}SampleOut.xml
