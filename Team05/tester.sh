 #!/bin/bash

config=Debug
while getopts 'r' flag
do
    case "${flag}" in
        r) config=Release
    esac
done

readonly iteration1=./Tests05/iteration1/
readonly iteration2=./Tests05/iteration2/
readonly path_to_autotester=./Code05/out/build/x64-${config}/src/autotester/autotester.exe
readonly invalid_q_path=invalidQuery/
readonly invalid_s_path=invalidSource/
readonly no_constr_path=noConstraintsQuery/
readonly one_constr_path=oneConstraintQuery/
readonly two_constr_path=twoConstraintsQuery/
readonly milestoneBugs_path=./Tests05/iteration1/conditionsInWhile/
readonly iteration1_output_folder=./Code05/testOutputs/iteration1/
readonly iteration2_output_folder=./Code05/testOutputs/iteration2/
readonly sample_path=./Tests05/

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

for i in calls modifies
do
    ${path_to_autotester} ${iteration2}${one_constr_path}${i}Only_source.txt ${iteration2}${one_constr_path}${i}Only_queries.txt ${iteration2_output_folder}${i}onlyOut.xml
done

echo "Starting two constraint query test cases"
for i in parent uses follows modifies
do
    ${path_to_autotester} ${iteration1}${two_constr_path}${i}AndPattern_source.txt ${iteration1}${two_constr_path}${i}AndPattern_queries.txt ${iteration1_output_folder}${i}AndPatternOut.xml
done

# For random testing, modify Sample_queries.txt and Sample_source.txt
echo "starting local sample test cases"
${path_to_autotester} ${sample_path}Sample_source.txt ${sample_path}Sample_queries.txt ${iteration1_output_folder}SampleOut.xml
