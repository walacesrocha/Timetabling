tx[1]=0
tx[2]=0.25
tx[3]=0.5
tx[4]=0.75
tx[5]=1

for i in {1..7} 
do
	nome=$i
	if [ $i -lt 10 ]; then
		nome="0$nome" ;
	fi
	for seed in {1..5}
	do
		for n in {1..5} 
		do
			./grasp instancias/comp$nome.ctt bl=sa ti=2 tf=0.001 beta=0.999 txap=${tx[$n]} seed=$seed timeout=320 >> testesTx/comp$nome.$n.sol
		done
	done
done
