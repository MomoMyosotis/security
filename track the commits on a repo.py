import requests
import schedule
import os
import time
import json
from datetime import datetime

# controlla se il file esiste
def check_file():
    # to check the file
    quack = input("file and extension: ")
    try:
        with open(quack) as file:
            data = json.load(file)
    except Exception as x:
        print("an error has occured with the file. error 01\nerror -> " + str(x))
        print("__________________________________________________________________")
        return
    else:
        return data, quack

# to check the data
def checking_data(quack, bl):
    # in case data needs to be changed
        print_dict(quack)
        change = input("wanna change it? if yes print anything\nchoice: ")
        change = bool(change)
        if change == True:
            quack["OWNER"] = input("new repo's owner USERNAME: ")
            quack["REPO"] = input("new repo's link: ")
            quack["LOG_FILE"] = input("new where to save the data: ")

            # lsc -> last seen commit

            print_dict(quack)
            return quack
        else:
            print("we will work with those data then.\n")
#            time.sleep(2)
#            os.system("clear")
            #   print_dict(quack)
            return quack

# saving the data if ever changed
def save_data(pru, bl):
    print("saving data...")
    try:
        with open(bl, "w") as file:
            # indent=4 rende leggibile meglio il documento
            json.dump(pru, file, indent=4)
    except Exception as e:
        print("error 02\n error -> " + str(e))
    else:
        print("change saved!")
        return pru

# stampa dizionario
def print_dict(dname):
    print("___________________________")
    for x,y in dname.items():
        print (f"{x} -> {y}")
    print("___________________________")

# get last recorded commit
def get_lrc(nuts):
    i = nuts["repo"]
    url = f"https://api.{i}/commits/main/"
    print (url)
    try:
        #   requests.get() prova ad ottenere una risposta dal server con timeout di 10 secondi
        response = requests.get(url, timeout=10)

        # in caso di errore nella connessione
        response.raise_for_status()

    # in caso di errori:
    except requests.exceptions.HTTPError as e:
        print("_______________________________________________")
        return (print("error 03\n HTTP error -> " + str(e))), e

    except requests.exceptions.RequestException as o:
        print("_______________________________________________")
        return print("error 04\nerror -> " + str(o)), o

    # se per qualche allineamento astrale va tutto bene
    else:
        print ("la richiesta è andata a buon fine\nrecupero dati...\n")

        commits = response.json()
        last_commit = commits[0]
        author = last_commit['commit']['author']['name']
        date = last_commit['commit']['author']['date']
        return author, date

# flow
def main():
    diz, fname = check_file()
    # converte diz da str a dict
    diz = dict(diz)
    # controlla file e dati
    if diz is None:
        return

    deez = checking_data(diz, fname)

    autore, data = get_lrc(deez)
    print("_______________________________________________")
#    print(f"autore is {autore}")
#    print(f"data is {data}")


main()
# last line
