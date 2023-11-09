#-----------------------------------------------------------------------------------------------------#
#
#                      Programme de simulation du parachute de la mini-fusée
#
#-----------------------------------------------------------------------------------------------------#

## Notes
'''
dernière modification par M SEAUVE le 08/02/2020

To a définir en entrée ( décollage de la mini-fusée)
Ta et Tb aussi (resp. T1 et T2)
T temps théorique de l'apogé d'après les simulation
deltaT écart entre chaque donnée temporel en entrée

Pour pouvoir utiliser le programme entrer ces commandes dans le shells (regarde en haut a droite) :

pip install xlrd    pour installer le module (pas sur de la syntaxe notamment pour le "pip")
insert(0, "C:/Users/seauv/Documents/ELISA/Divers/donnes.xlsx") modifier le chemin selon l'emplacement du fichier excel

table[y][x]   x et y placé de la facon la plus courante en mathématique



'''
## Blibliothèques

import xlrd # pour interragir avec excel
import os
import sys
import turtle # pour le graph
import random
import math

## Données

To = 0.1
deltaT = 0.1
Ta = 2
Tb = 3
t = 0
parachute = 0
altitudeActu = 0
start = 0
tLoc = 0
tPara = 0   # instant t local de déclenchement du parachute
hmax = 0   # altitude maximale atteinte
errorCapt = 20
compteurMax = 6     # nombre de valeur en descente nécéssaire a l'ouverture du parachute
compteur = 0
nbMME = 10

alpha = 0.78 # variables pour le lissage
numerateur = 0
denominateur = 0

tmax = 4000   # sert a donné une limite temporel a la simulation
iDec = 0 # adresse au décollage (pour donnesDeVol)

h = []
hLisse = []
time = []

donnesDeVol = [[0 for j in range(0,1)] for i in range(0,3)]


## Récupération des données du doc excel

desktop=os.path.join(os.environ['HOMEPATH'], 'Desktop')
#print(desktop) 

sys.path.insert(0, "C:/Users/seauv/Documents/ELISA/Divers/donnes.xlsx")   
#print(sys.path)

path = "C:/Users/seauv/Documents/ELISA/Divers/donnes.xlsx"

classeur = xlrd.open_workbook(path)

nom_des_feuilles = classeur.sheet_names()
 
# Récupération de la première feuille
feuille = classeur.sheet_by_name(nom_des_feuilles[0])

''' 
print ("A1: {}".format(feuille.cell_value(0, 0)))
print ("B1: {}".format(feuille.cell_value(0, 1)))
print ("C1: {}".format(feuille.cell_value(0, 2)))
a = feuille.col(0)

print(a[5])
'''


for s in classeur.sheets():
    table=[] # Creation d'une liste de lignes
    print ('Feuille:',s.name)
    for ligne in range(s.nrows):
        table.append([]) # Creation d'une liste par ligne pour les colonnes : table[ligne][colonne]
        for col in range(s.ncols):
            table[ligne].append(s.cell(ligne,col).value)
    #print (table)

deltaT = feuille.cell_value(2, 0) - feuille.cell_value(1, 0)
print("deltaT : {}".format(deltaT))
#tmax = feuille.cell_value(feuille.nrows-1, 0) + To
print("tmax : {}".format(tmax))
To = feuille.cell_value(1, 2)
print("T0 : {}".format(To))
Ta = feuille.cell_value(1, 3)
print("T1 : {}".format(Ta))
Tb = feuille.cell_value(1, 4)
print("T2 : {}".format(Tb))

for i in range(1,feuille.nrows):
    
    a = str(feuille.cell(i,1))
    a = a.replace('number:','')
    a = float(a)
    h.append(a)
    h[i-1] = h[i-1] + errorCapt * random.random()
    
    
for i in range(1,feuille.nrows):
    
    b = str(feuille.cell(i,0))
    b = b.replace('number:','')
    b = float(b)
    time.append(b)


try:
    alpha = feuille.cell_value(1, 6)
except:
    pass

try:
    nbMME = int(feuille.cell_value(1, 7))
except:
    pass

try:
    compteurMax = int(feuille.cell_value(1, 8))
except:
    pass



## Corps du programme


print("\n\nt global h   t locale")

for i in range(0,int(tmax)):
    
    #print(i)
    
    t = time[i]
    #print(round(t,2),"   ",round(h[i]))
    

    
    
    
    
    
    altitudePrec = altitudeActu
    altitudeActu = h[i-iDec]   # ces 2 lignes permettent de savoir si la fusée est en train de descendre
    
        
    if altitudeActu > hmax:
        # On ajuste si besons la hauteur maximale atteinte par la fusée
        hmax = altitudeActu
    
    # gère le décollage (a remplacer a terme par la détection d'une variable ou d'un signal indiquant le décollage)
    if t > To and start == 0:
        start = 1
        print("décollage")
        iDec = i
        
    
    # les deux if suivant gère le déclenchement du parachute    
    
    if t-To < Tb and t-To > Ta and parachute == 0 and start == 1:
        
        
        if altitudeActu < altitudePrec:
            
            compteur = compteur + 1
            
            if compteur >= compteurMax:
                
                parachute = 1
                tPara = tLoc
                print("Parachute déclenché")
                print(donnesDeVol[2][i-iDec],"zer    ",0.98*max(hLisse),"   ",max(hLisse))
                
        else:
            
            compteur = 0
        #print(donnesDeVol[2][i-iDec],"zer    ",0.95*hmax,"   ",hmax)
        if donnesDeVol[2][i-iDec] <= 0.98*max(hLisse):
            print(donnesDeVol[2][i-iDec],"zer    ",0.98*max(hLisse),"   ",max(hLisse))
            parachute = 1
            tPara = tLoc
            print("Parachute déclenché")
        
        
        
    if t-To > Tb and parachute == 0 and start == 1:
        
        parachute = 1
        tPara = tLoc
        print("Parachute déclenché\n")
        print(donnesDeVol[2][i-iDec],"zer    ",0.98*max(hLisse),"   ",max(hLisse))
  



    
    
    # gère l'affichage des données
    
    if start == 0:
        
        print(round(t,2),"   ","0.0")
    
    else:
        
        numerateur = 0
        denominateur = 0
        kmin = i-iDec-nbMME
        if kmin < 0:
            kmin = 0
    
        for k in range(kmin,i-iDec):
            
            numerateur = numerateur + h[k]*pow(alpha,k-kmin)
        
            denominateur = denominateur + pow(alpha,k-kmin)
            
            
            
        if denominateur == 0:
            hLisse.append(0)
            donnesDeVol[2].append( round(hLisse[i-iDec]) )
            
        else:
            
            hLisse.append(numerateur/denominateur)
            donnesDeVol[2].append( round(hLisse[i-iDec]) )
        

        
        

        print(round(t,2),"   ",round(h[i-1]),"   ",round(tLoc,2))
        tLoc = t-To
        
        donnesDeVol[0].append( round(tLoc,2) )
        donnesDeVol[1].append( round(h[i-iDec]) )


    

    if t-To > Tb and start == 1 and h[i-iDec] < 0:

        
        break

    
    
    
## Graphe


print("hauteur max atteint : {}".format(round(hmax,2)))
print("parachute déclencher a : {}".format(round(tPara,2)))

dotcount = 0

if tmax > len(donnesDeVol[0]):
    
    tmax = len(donnesDeVol[0])


tcadre = 400  # taille du cadre

try:             # désolé pour ces 5 lignes ignobles
    turtle.setup(1280, 900)

except:
    pass
    
turtle.setup(1280, 900)

turtle.speed(10000)

turtle.up()                   # cadre du graphe
turtle.goto(tcadre,tcadre)
turtle.down()
turtle.goto(-tcadre,tcadre)
turtle.goto(-tcadre,-tcadre)
turtle.goto(tcadre,-tcadre)
turtle.goto(tcadre,tcadre)


turtle.up()  

turtle.goto( ((donnesDeVol[0][0])/tLoc)*2*tcadre-tcadre , (donnesDeVol[1][0]/hmax)*2*tcadre-tcadre )
turtle.down()


for j in range(1,tmax-1,5):
    
    
    turtle.goto( ((donnesDeVol[0][j])/tLoc)*2*tcadre-tcadre , (donnesDeVol[1][j]/hmax)*2*tcadre-tcadre )

    #print(((donnesDeVol[0][j])/tLoc)*2*tcadre-tcadre)
    #print((donnesDeVol[1][j]/hmax)*2*tcadre-tcadre ,"\n")
    
    
    #print(j,"   ",int(tPara/deltaT) )
    
    if j > int(tPara/deltaT) and parachute == 1:
        
        turtle.dot(15,'red')
        parachute = 2
    
    
    if j > int(Ta/deltaT) and dotcount == 0:
        
        turtle.dot(8,'blue')
        dotcount = 1
        
    
    if j > int(Tb/deltaT) and dotcount == 1:
        
        turtle.dot(8,'blue')
        dotcount = 2


turtle.color('blue')

turtle.up()  

turtle.goto( ((donnesDeVol[0][0])/tLoc)*2*tcadre-tcadre , (hLisse[0]/hmax)*2*tcadre-tcadre )
turtle.down()


for j in range(1,len(hLisse)-1,1):
    #print(hLisse[j],"  ",j, "   ",len(hLisse))
    turtle.goto( ((donnesDeVol[0][j])/tLoc)*2*tcadre-tcadre , (hLisse[j]/hmax)*2*tcadre-tcadre )
    











## Fin du Code


turtle.exitonclick()   # permet de laisser le graph en place après sa réalisation (de base la fenetre se ferme quand le code a finit de tourner)




#https://stackoverflow.com/questions/24988448/how-to-draw-vertical-lines-on-a-given-plot-in-matplotlib









        
    
    





