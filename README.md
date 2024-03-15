# Tema 4 - Client Web. Comunicatie cu REST API

 - Nume: Varzaru Vlad-Ionut
 - Grupa : 325CD

Pentru aceasta tema am folosit ca schelet laboratorul 9, caruia i-am adaugat si functia  'compute_delete_request' si am adaugat parametrul token la functiile de POST si GET. 

--Comenzi--
Clientul are implelentate 9 comenzi pe care le poate interpreta in functie de datele introduse in consola astfel:

1. "register"
---->Realizeaza o cerere de inregistrare cu user-ul si parola introduse de la tastatura si trimite un POST request. User-ul si parola nu pot fi goale si nici nu pot contine spatii,  
      	serverul va intoarce o eroare in aceste cazuri.
2. "login"
---->Comanda de login functioneaza la fel ca cea de register, are aceleasi restrictii cand vine vorba de user si parola, iar serverul va intoarce o eroare in cazul in care username-ul 
	sau parola sunt gresite sau daca utilizatorul este deja autentificat. Daca logarea se efectueaza cu succes, clientu va retine un cookie pentru autentificare.
3. "enter_library"
---->La introducerea acestei comezni, serverul poate returna 2 raspunsuri: 
	- daca utilizatorul este logat, va intoarce codul 200 si va genera un token JWT pe care clientul il retine si va putea accesa biblioteca
	-daca utilizatorul nu este logat, biblioteca nu poate fi accesata si va intoarce codul 401.
4. "get_books"
---->Daca utilizatorul nu este logat, se va afisa o eroare prin care i se cere sa se autentifice, iar daca este va verifica existenta tokenului pentru librarie. Daca are acces la biblioteca 
	vor fi afisate toate cartile, in caz contrar serverul va intoarce codul 403-Forbidden.
5. "get_book"
---->Dupa introducerea acestei comenzi, utilizatorului i se va cere sa introduca si id-ul asociat cartii dorite. Se verifica daca id-ul introdus este un numar, daca utilizatorul este logat
	si daca are acces la biblioteca. Daca toate conditiile sunt indeplinite, se vor afisa toate detaliile despre cartea cu id-ul respectiv.
6. "add_book"
---->Pentru aceasta comanda utilizatorul va trebui sa introduca toate datele asociate unei carti (title, author, genre, publisher, page count). Datele introduse nu pot fi goale, iar
	page count accepta doar numere. La fel ca la comenzile dinainte, trebuie sa fim logati si sa avem acces la librarie pentru a adauga cu succes cartea.
7. "delete_book"
---->Asemanator cu "get_books", aceasta comanda necesita introducerea id-ului asociat cartii pe care dorim sa o stergem din librarie, care trebuie sa fie un id valid. Daca nu avem 
	acces la librarie, serverul va intoarce codul 403-Forbidden, iar daca avem va intoarce 200, ceea ce inseamna ca stergerea s-a realizat cu succes.
8. "logout"
---->Daca utilizatorul nu este logat va intoarce eroare, iar daca este, clientul va sterge cookie-ul asociat conectarii precum si token-ul.
9. "exit"
---->La introducerea acestei comenzi clientul se inchide.

--Functii auxiliare--
Pentru implementarea clientului am creat cateva functii auxiliare:
- "get_cookie", "get_token" si "compute_books" extrag datele de la raspunsul serverului
- "read_book_info" citeste datele cartii si le valideaza

--Libraria JSON--
Pentru a parsa raspunsurile primite de la server am ales sa folosesc libraria parson implementata de kgabis (https://github.com/kgabis), deoarece codul este scris in C, am mai lucrat cu aceasta librarie si mi s-a parut mai usor de utilizat.

--Rulare--
Tema poate fi rulata prin comanda "make run" care compileaza codul si executa ./client.