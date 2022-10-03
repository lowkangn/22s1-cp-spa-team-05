 #!/bin/bash

config=Debug
while getopts 'r' flag
do
    case "${flag}" in
        r) config=Release
    esac
done

readonly path_to_autotester=./Code05/out/build/x64-${config}/src/autotester/autotester.exe
readonly invalid_q_path=./Tests05/iteration1/invalidQuery/
readonly invalid_s_path=./Tests05/iteration1/invalidSource/
readonly no_constr_path=./Tests05/iteration1/noConstraintsQuery/
readonly one_constr_path=./Tests05/iteration1/oneConstraintQuery/
readonly two_constr_path=./Tests05/iteration1/twoConstraintsQuery/
readonly milestoneBugs_path=./Tests05/iteration1/conditionsInWhile/
readonly output_folder=./Code05/testOutputs/
readonly sample_path=./Tests05/

echo "Using autotester at: "${path_to_autotester}

echo "Starting invalid query test cases"
for i in Declaration Follows Modifies Parent Select Uses Pattern
do	
       ${path_to_autotester} ${invalid_q_path}invalid$i\_source.txt ${invalid_q_path}invalid$i\_queries.txt ${output_folder}invalidQueryOut$i.xml
done

echo "Starting invalid source test cases"
for i in {1..19}
do	
       ${path_to_autotester} ${invalid_s_path}invalidSource$i\_source.txt ${invalid_s_path}invalidSource$i\_queries.txt ${output_folder}invalidSourceOut$i.xml
done

echo "Starting milestone test cases"
for i in {1..3}
do	
       ${path_to_autotester} ${milestoneBugs_path}whileStatementTest$i\_source.txt ${milestoneBugs_path}whileStatementTest$i\_queries.txt ${output_folder}whileStatementTest$i.xml
done

echo "Starting no constraints query test cases"
${path_to_autotester} ${no_constr_path}noConstraints_source.txt ${no_constr_path}noConstraints_queries.txt ${output_folder}outNoConstraints.xml

echo "Starting one constraint query test cases"
for i in follows modifies parent pattern uses
do
	${path_to_autotester} ${one_constr_path}${i}Only_source.txt ${one_constr_path}${i}Only_queries.txt ${output_folder}${i}onlyOut.xml
done

echo "Starting two constraint query test cases"
for i in parent uses follows modifies
do
${path_to_autotester} ${two_constr_path}${i}AndPattern_source.txt ${two_constr_path}${i}AndPattern_queries.txt ${output_folder}${i}AndPatternOut.xml
done

# For random testing, modify Sample_queries.txt and Sample_source.txt
echo "starting local sample test cases"
${path_to_autotester} ${sample_path}Sample_source.txt ${sample_path}Sample_queries.txt ${output_folder}SampleOut.xml
