#!/bin/bash

# Verifică dacă numărul de argumente este corect
if [ "$#" -ne 1 ]; then
    echo "Utilizare: $0 <caracter>"
    exit 1
fi

caracter=$1
contor=0

# Funcție care verifică dacă o linie este o propoziție corectă
function este_propozitie_corecta {
    local linie=$1
    [[ $linie =~ ^[A-Z] && $linie =~ [!.?]$ && ! $linie =~ ,\  && ! $linie =~ ",si" && $linie =~ $caracter ]]
}

# Citirea liniilor de la intrarea standard
while IFS= read -r linie || [[ -n "$linie" ]]; do
    if este_propozitie_corecta "$linie"; then
        ((contor++))
    fi
done

# Afișarea contorului
echo $contor
