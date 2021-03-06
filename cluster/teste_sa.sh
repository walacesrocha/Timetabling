#!/bin/sh
# Este script roda um unico job em SGE
# Ele pode ser executado pelo comando SGE abaixo:
# qsub um_job.sh

# A linha abaixo nao e' um comentario. Ela especifica que o que vem
# apos ela sera invocado com o /bin/sh shell
#$ -S /bin/sh

# Coloca os arquivos de saida e de erro do seu job no diretorio .
#$ -cwd
#--------------------------------
# exemplo: (altere a partir daqui)

# Parametros:
#	1: Instancia
#	2: Temperatura inicial
#	3: Rho
#	4: Nr. candidato
#	5: Fase

../sa ../instancias/${1}.ctt ${2} ${3} 0.995 >> resultados_sa/stat_${1}_C${4}_F${5}.txt

# Fim do script

