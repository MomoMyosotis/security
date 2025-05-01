from PyPDF2 import PdfReader, PdfWriter

# what we do
print("this code will allow you to extract a pdf from a given pdf.
\nyou'll be able to specify which pages you'll need and how many copies of each one you'll need.
\nuseful for documents that may require more copies of the same form")

# --- indications ---
nome_flle = input("how's the file named?\nname: ")
repeating = input("which pages repeat how many times?\npage: ")

# --- config ---
dynamic = {}
input_file = f"{nome_flle}.pdf"
output_file = f"output_{nome_flle}.pdf"

terminator = None

counter = 0
while terminator is None:

    quackie = input(f"insert {counter +1}'s page")
    nooty = input(f"how many copies?\nnum: ")
    quackie = int(quackie)
    nooty = int(nooty)
    dynamic[quackie] = nooty
    counter +=1
    terminator = input("type anything if you finished pointingwhat you need")

reader = PdfReader(input_file)
writer = PdfWriter()

for x in dynamic:

    # Note: pages start from 0 not 1
    page = dynamic[x] - 1

    num = dynamic[x]
    num = int(num)
    cycle = 0
    while cycle != num:
        # adding the page
        writer.add_page(reader.pages[page])
        cycle +=1

# Scrivo il nuovo file
with open(output_file, "wb") as f_out:
    writer.write(f_out)

print(f"Done!\nFile saved as: {output_file}")

# last line
