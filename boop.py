import tkinter as tk
from tkinter import filedialog, messagebox, ttk
import fitz  # PyMuPDF per il PDF
import random
import re  # Per lavorare con espressioni regolari
import os
import sys

# Mappatura dei caratteri speciali
caratteri_speciali = {
    '|': '01', '!': '02', '"': '03', '£': '04', '$': '05', '%': '06', '&': '07',
    '/': '08', '(': '09', ')': '10', '=': '11', '?': '12', "'": '13', '^': '14',
    '+': '15', '*': '16', '{': '17', '}': '18', '[': '19', ']': '20', '-': '21',
    '_': '22', '.': '23', ',': '24', ':': '25', ';': '26', '<': '27', '>': '28',
    '@': '29', '#': '30', '|': '31', '!': '32', '"': '33', '£': '34', '$': '35', 
    '%': '36', '&': '37', '/': '38', '(': '39', ')': '40', '=': '41', '?': '42',
    "'": '43', '^': '44', '+': '45', '*': '46', '{': '47', '}': '48', '[': '49', 
    ']': '50', '-': '51', '_': '52', '.': '53', ',': '54', ':': '55', ';': '56', 
    '<': '57', '>': '58', '@': '59', '#': '60'
}

# Impostazioni della barra di progresso
progress_reset_delay = 2000  # Tempo di attesa prima del reset in millisecondi
progress_reset_timer = None  # Timer per il reset

# Funzione per caricare il contenuto del PDF
def carica_pdf(file_pdf):
    if not os.path.exists(file_pdf):
        raise FileNotFoundError(f"Errore: il file '{file_pdf}' non esiste.")
    
    documento = fitz.open(file_pdf)
    contenuto = []
    for pagina_num in range(documento.page_count):
        pagina = documento.load_page(pagina_num)
        testo = pagina.get_text("text")
        contenuto.append(testo.split('\n'))
    return contenuto

# Funzione per separare parola e punteggiatura
def separa_parola_punteggiatura(parola):
    parola_pulita = re.sub(r'[^\w\s]', '', parola)
    punteggiatura = re.sub(r'[\w\s]', '', parola)
    return parola_pulita, punteggiatura

# Reset della barra di progresso
def reset_progress_bar():
    progress_var.set(0)

# Funzione per ritardare il reset della barra di progresso
def delayed_progress_reset():
    global progress_reset_timer
    if progress_reset_timer is not None:
        root.after_cancel(progress_reset_timer)
    progress_reset_timer = root.after(progress_reset_delay, reset_progress_bar)

# Funzione di codifica
def codifica(frase, file_pdf, progress_var):
    try:
        contenuto = carica_pdf(file_pdf)
    except FileNotFoundError as e:
        messagebox.showerror("Errore", str(e))
        return None
    except Exception as e:
        messagebox.showerror("Errore", f"Si è verificato un errore durante il caricamento del PDF: {str(e)}")
        return None

    codificato = []
    parole = frase.split()

    for idx, parola in enumerate(parole):
        try:
            parola_pulita, punteggiatura = separa_parola_punteggiatura(parola)
            parola_pulita = parola_pulita.lower()

            pagina_trovata = False
            pagine_analizzate = 0
            max_pagine = len(contenuto)

            # Tentativo di ricerca più preciso
            while not pagina_trovata and pagine_analizzate < max_pagine:
                numero_pagina = random.randint(0, max_pagine - 1)
                pagina = contenuto[numero_pagina]

                trovato_nella_pagina = False
                for numero_riga, riga in enumerate(pagina):
                    riga_pulita = re.sub(r'[^\w\s]', '', riga).strip().lower()
                    parole_riga = riga_pulita.split()

                    if parola_pulita in parole_riga:
                        posizione_parola = parole_riga.index(parola_pulita) + 1
                        codificato.append(f"{numero_pagina+1:04d}{numero_riga+1:02d}{posizione_parola:02d}")
                        if punteggiatura:
                            for char, cod in caratteri_speciali.items():
                                if char in punteggiatura:
                                    if punteggiatura.startswith(char):
                                        codificato[-1] += cod + '1'
                                    elif punteggiatura.endswith(char):
                                        codificato[-1] += cod + '2'
                        pagina_trovata = True
                        break

                pagine_analizzate += 1

                # Retry se la parola non è trovata
                if not pagina_trovata and pagine_analizzate == max_pagine:
                    messagebox.showerror("Errore", f"La parola '{parola}' non è stata trovata nel PDF.")
                    return None

            # Aggiorna la barra di avanzamento
            progress_var.set((idx + 1) / len(parole) * 100)
            root.update_idletasks()

        except Exception as e:
            messagebox.showerror("Errore durante la cifratura", f"Si è verificato un errore durante la codifica della parola '{parola}': {str(e)}")
            return None

    return '-'.join(codificato)


# Funzione di decodifica
def decodifica(codifica_input, file_pdf, progress_var):
    try:
        contenuto_pdf = carica_pdf(file_pdf)
    except FileNotFoundError as e:
        messagebox.showerror("Errore", str(e))
        return None
    except Exception as e:
        messagebox.showerror("Errore", f"Si è verificato un errore durante il caricamento del PDF: {str(e)}")
        return None

    decodificato = []
    numeri_codifica = codifica_input.split('-')

    for idx, numero in enumerate(numeri_codifica):
        try:
            if len(numero) < 8:
                raise ValueError("Codifica non valida.")
                
            pagina_num = int(numero[:4]) - 1
            riga_num = int(numero[4:6]) - 1
            parola_num = int(numero[6:8]) - 1
        except ValueError as e:
            messagebox.showerror("Errore", f"Codifica non valida o errore nei dati: {str(e)}")
            return None

        punteggiatura = ''
        if len(numero) > 8:
            punteggiatura_info = numero[8:]
            for char, cod in caratteri_speciali.items():
                if punteggiatura_info.startswith(cod + '1'):
                    punteggiatura = char
                elif punteggiatura_info.endswith(cod + '2'):
                    punteggiatura = char

        try:
            parola = contenuto_pdf[pagina_num][riga_num].split()[parola_num]
            parola_pulita = re.sub(r'[^\w\s]', '', parola)
            decodificato.append(parola_pulita + punteggiatura)
        except IndexError:
            decodificato.append("[errore]")

        # Aggiorna la barra di avanzamento
        progress_var.set((idx + 1) / len(numeri_codifica) * 100)
        root.update_idletasks()

    return ' '.join(decodificato)


# Selezione del file PDF
def seleziona_file():
    return filedialog.askopenfilename(title="Seleziona un file PDF", filetypes=[("PDF files", "*.pdf")])

# Eventi di codifica e decodifica
def codifica_evento():
    frase = frase_entry.get()
    file_pdf = seleziona_file()
    if not frase or not file_pdf:
        messagebox.showerror("Errore", "Frase o file PDF non selezionati!")
        return
    progress_var.set(0)
    codificato = codifica(frase, file_pdf, progress_var)
    if codificato:
        output_text.delete(1.0, tk.END)
        output_text.insert(tk.END, codificato)

def decodifica_evento():
    codifica_input = codifica_input_entry.get()
    file_pdf = seleziona_file()
    if not codifica_input or not file_pdf:
        messagebox.showerror("Errore", "Codifica o file PDF non selezionati!")
        return
    progress_var.set(0)
    decodificato = decodifica(codifica_input, file_pdf, progress_var)
    if decodificato:
        output_text.delete(1.0, tk.END)
        output_text.insert(tk.END, decodificato)

# Verifica del sistema operativo
if sys.platform == "darwin":
    messagebox.showwarning("Attenzione", "Questa applicazione potrebbe non essere completamente compatibile con macOS.")

# Creazione dell'interfaccia grafica
root = tk.Tk()
root.title("Codifica/Decodifica PDF")

frase_label = tk.Label(root, text="Frase da codificare:")
frase_label.pack(padx=10, pady=5)

frase_entry = tk.Entry(root, width=40)
frase_entry.pack(padx=10, pady=5)

codifica_button = tk.Button(root, text="Codifica", command=codifica_evento)
codifica_button.pack(padx=10, pady=5)

codifica_input_label = tk.Label(root, text="Codifica da decodificare:")
codifica_input_label.pack(padx=10, pady=5)

codifica_input_entry = tk.Entry(root, width=40)
codifica_input_entry.pack(padx=10, pady=5)

decodifica_button = tk.Button(root, text="Decodifica", command=decodifica_evento)
decodifica_button.pack(padx=10, pady=5)

progress_var = tk.DoubleVar()
progress_bar = ttk.Progressbar(root, variable=progress_var, maximum=100, length=300)
progress_bar.pack(padx=10, pady=10)

output_text = tk.Text(root, height=10, width=50)
output_text.pack(padx=10, pady=5)

root.mainloop()
