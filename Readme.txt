George Neacsu - 331CC
george@neacsugeorge.com

Progresul se poate vedea si aici:
https://github.com/neacsugeorge/APD-Tema1/commits/master

Coding style:
Imi cer scuze in avans, m-am apucat putin cam tarziu de Tema1
si nu am avut timp sa-l fac sa arate mai decent.

Part1:
Initial am ales sa tin imaginea intr-o matrice 2D pentru grayscale
si 3d pentru color.

Am facut citirea & scrierea (cu fscanf/fprintf).
Am inceput sa rezolv functia de resize, initial o parte alocare pentru
martricea rezultat o faceam inainte de crearea thread-urilor. De asemenea
o greseala era ca pentru calcul imi completam un vector de valori (cu for in for)
apoi le adunam cu un alt for si imparteam sa aflu valoare pixelului rezultat.
Functiona corect, dar era foarte slow.

Prima optimizare a constat in renuntarea la acea functie care calcula valoarea
pixelului rezultat si mutarea logicii direct in threadFunction. Pe langa asta
am renuntat la crearea unui vector de valori si am inceput sa fac suma direct.
De asemenea am mutat alocarea pentru imaginea rezultat tot in threadFunction.

A doua optimizare a constat in inlocuirea fscanf si fprintf pentru pixeli cu
fread si fwrite;

A treia optimizare a constat in adaugarea de variabile pentru calculele
care se repetau, numele nu sunt foarte sugestive, dar au niste sens. De aceasta
data, am schimbat si structura functiei threadFunction. Inainte era un for in for
si cca 4-5 decizii in interiorul for-urilor asa ca am visat urat si m-am gandit
ca ar fi mai optim sa inversez asta, asa ca am mutat deciziile primele, si pentru
fiecare ramura dintr-o decizie am pus un for in for.

A patra optimizare a constat in renuntarea la o matrice 3D pentru imaginile color
si folosirea unei matrice 2D care e de 3 ori mai lata. Asta a imbunatatit foarte
mult I/O -ul din functiile de citire/scriere. Am ajustat apoi logica din threadFunction
si apoi am luat punctajul maxim pe checker. De asemenea, timpi rezultati din checker
par foarte buni.

Part2:
Aceasta parte a fost facuta inainte de optimizarile pentru Part1, asadar foloseste
fprintf pentru scriere.
Am copiat si adaptat rapid ce mi-a trebuit de la Part1. Creat threadFunction,
creat thread-uri.
Am implementat formula si apoi am stat si am facut bruteforce + uitat pe forum + iar 
bruteforce pana sa ajunga sa dea la fel precum in fisierele de referinta :'(.
Am stat vreo 3-4h sa mut chestii mai la stanga sau mai la dreapta in formula pana sa mearga.
Ulterior am inteles ce faceam diferit, ca mai intai trebuie sa scalez punctele si apoi sa
fac distanta, nu sa scalez distanta.
Nu am stat sa mai fac optimizari si aici deoarece oricum am intarziere destula.

O zi/seara frumoasa!