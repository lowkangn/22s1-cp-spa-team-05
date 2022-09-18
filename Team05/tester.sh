#!/bin/bash

readonly path_to_autotester=./Code05/out/build/x64-Debug/src/autotester/autotester.exe
readonly invalid_q_path=./Tests05/iteration1/invalidQuery/
readonly invalid_s_path=./Tests05/iteration1/invalidSource/
readonly no_constr_path=./Tests05/iteration1/noConstraintsQuery/
readonly one_constr_path=./Tests05/iteration1/oneConstraintQuery/
readonly two_constr_path=./Tests05/iteration1/twoConstraintsQuery/
readonly output_folder=./Code05/testOutputs/


echo "Starting invalid query test cases"
for i in Declaration Follows Modifies Parent Select Uses
do	
       ${path_to_autotester} ${invalid_q_path}invalid$i\_source.txt ${invalid_q_path}invalid$i\_queries.txt ${output_folder}invalidQueryOut$i.xml
done

echo "Starting invalid source test cases"
for i in {1..19}
do	
       ${path_to_autotester} ${invalid_s_path}invalidSource$i\_source.txt ${invalid_s_path}invalidSource$i\_queries.txt ${output_folder}invalidSourceOut$i.xml
done

echo "Starting no constraints query test cases"
${path_to_autotester} ${no_constr_path}noConstraints_source.txt ${no_constr_path}noConstraints_queries.txt ${output_folder}outNoConstraints.xml

echo "Starting one constraint query test cases"
for i in follows modifies parent pattern Uses
do
	${path_to_autotester} ${one_constr_path}${i}Only_source.txt ${one_constr_path}${i}Only_queries.txt ${output_folder}${i}onlyOut.xml
done

#TODO: ADD THE REST (follows modifies uses)
echo "Starting two constraint query test cases"
for i in parent
do
${path_to_autotester} ${two_constr_path}${i}AndPattern_source.txt ${two_constr_path}${i}AndPattern_queries.txt ${output_folder}${i}AndPatternOut.xml
done

