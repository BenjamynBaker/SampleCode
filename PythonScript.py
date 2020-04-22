import json as json
import csv as csv
import pandas as pd

# This Python Script was used to cleanse and sort data for the AirBnB chloropleth out output 
# a CSV with totals for each neighbourhood
# In hindsight, using a hashtable to store the neighbourhoods would have been a more efficient solution

# Get the names of all available neighbourhoods
with open("Community Boundaries.geojson") as file:
    data = json.load(file)


# Create a list for each neighbour hood with the properties:
# [ name, count, numratings, rating, people, bedrooms, price]
name_list = []
for item in data["features"]:
    name_list.append(item["properties"]["name"])

name_list = [[i] for i in name_list]

#initialize list values
for item in name_list:
    #count
    item.append(0)
    # num ratings
    item.append(0)
    #rating
    item.append(0)
    #people
    item.append(0)
    # bedrooms
    item.append(0)
    #price
    item.append(0)
    #crime
    item.append(0)
    #population
    item.append(0)

# print(name_list)



format1 = ["tomslee_airbnb_calgary_0542_2016-08-22.csv", "tomslee_airbnb_calgary_0554_2016-09-11.csv",
           "tomslee_airbnb_calgary_0579_2016-09-21.csv", "tomslee_airbnb_calgary_0624_2016-10-27.csv",
           "tomslee_airbnb_calgary_0679_2016-12-09.csv", "tomslee_airbnb_calgary_0724_2016-12-25.csv",
           "tomslee_airbnb_calgary_0870_2017-02-12.csv", "tomslee_airbnb_calgary_0943_2017-03-11.csv"
           ]

format2 = ["tomslee_airbnb_calgary_1055_2017-04-10.csv", "tomslee_airbnb_calgary_1203_2017-05-08.csv",
           "tomslee_airbnb_calgary_1326_2017-06-10.csv", "tomslee_airbnb_calgary_1443_2017-07-10.csv",
           "tomslee_airbnb_calgary_1558_2017-08-10.csv"]


format3 = ["Community_Crime_and_Disorder_Statistics (1).csv", "Community_Crime_and_Disorder_Statistics (2).csv"]

# FORMAT 1:
# [ name, numrating, rating, people, bedrooms, price,]
for file in format1:
    df = pd.read_csv(file, header=0, usecols=[4, 5, 6, 7, 8, 9])
    dfArray = df.to_numpy()
    dfArray = dfArray.tolist()
    # print(dfArray)


    #Add values to list
    for item in dfArray:
        for nbhood in name_list:
            if item[0] == nbhood[0]:
                #count
                nbhood[1] += 1
                #num ratings
                if item[1] != 0 and item[2] == item[2] and item[2] != 0:
                    nbhood[2] += item[1]
                #rating
                #check for nan
                if item[2] != item[2]:
                    nbhood[3] += 0
                else:
                    nbhood[3] += item[1] * item[2]
                #people
                nbhood[4] += item[3]
                #bedrooms
                if item[4] == item[4]:
                    nbhood[5] += item[4]
                #price
                if item[5] == item[5]:
                    nbhood[6] += item[5]



# FORMAT 2
for file in format2:
    df = pd.read_csv(file, header=0, usecols=[7, 8, 9, 10, 11, 13])
    dfArray = df.to_numpy()
    dfArray = dfArray.tolist()
    # print(dfArray)


    # add items to list
    for item in dfArray:
        for nbhood in name_list:
            if item[0] == nbhood[0]:
                #count
                nbhood[1] += 1
                #num ratings
                if item[1] != 0 and item[2] == item[2] and item[2] != 0 :
                    nbhood[2] += item[1]
                #rating
                #check for nan
                if item[2] != item[2]:
                    nbhood[3] += 0
                else:
                    nbhood[3] += item[1] * item[2]
                #people
                nbhood[4] += item[3]
                #bedrooms
                if item[4] == item[4]:
                    nbhood[5] += item[4]
                #price
                if item[5] == item[5]:
                    nbhood[6] += item[5]


# print(name_list)
# print(count)


#store averages instead of
for item in name_list:
    #avg review
    if item[2] != 0 :
        item[3] = round(item[3] / item[2], 2)
    #avg people
    if item[1] != 0:
        item[4] = round(item[4] / item[1], 2)
    #avg bedrooms
    if item[1] != 0:
        item[5] = round(item[5] / item[1], 2)
    #avg price
    if item[1] != 0:
        item[6] = round(item[6] / item[1], 2)


for file in format3:
    df = pd.read_csv(file, header=0, usecols=[1, 4, 5])
    dfArray = df.to_numpy()
    dfArray = dfArray.tolist()


    # print(dfArray)
    for item in dfArray:
        if (type(item[2]) == str):
            item[2] = item[2].replace(",", "")
        for nbhood in name_list:
            #add one for every matching entry
            if item[0] == nbhood[0]:
                nbhood[7] += item[1]
                #keep highest population
                if int(item[2]) > nbhood[8]:
                    nbhood[8] = int(item[2])




# Output CSV
header = ["Neighbourhood", "Count", "NumRatings", "Rating", "People", "Bedrooms", "Price", "Crime", "Population"]
with open("output.csv", 'w') as file:
    writer = csv.writer(file)
    writer.writerow(header)
    for item in name_list:
        writer.writerow(item)

