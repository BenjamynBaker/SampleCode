# This Python script was used to insert items into a database
# It reads in a list of json objects, formats the data, then
# sends the post request

import requests
import json
import os
import time

# match resorts to nearest airport
calgary_list = [5, 3, 4, 1, 2]
van_list = [7]
kelowna_list = [6]

# need to get the source id and match it to a airport
source_id_dict = {
    '96474': "YYZ",
    '96451': 'YYC',
    '96322': 'YVR',
    '96405': 'YXE',
    '96359': 'YWG',
    '96268': 'YUL',
    '95658': 'YHZ',
    '95847': 'YLW',
}


def send_request(data_object, resort_id):

    params = {'data': data_object, "resort_id": resort_id}
    print(params)
    r = requests.post("https://us-central1-opench370-project.cloudfunctions.net/api/flight", data=params)
    print("Finished sending object")


request_count = 0
for filename in os.listdir('./flightdata'):
    file = open( './flightdata/'+ filename, "r")
    print(filename)
    data_json = file.read()
    data_json = json.loads(data_json)


    if len(data_json) != 0:
        departureDate = data_json[0]['OutboundLeg']['DepartureDate']

        source = data_json[0]['OutboundLeg']['OriginId']
        destination = data_json[0]['OutboundLeg']['DestinationId']

        # Calculate avg cost
        count = 0
        total = 0
        for i in data_json:
            total += int(i['MinPrice'])
            count += 1

        avg = total/count

        # format body
        data_object = {
            'date' : filename[-15:-5],
            'source': source_id_dict[str(source)],
            'price': float(round(avg, 2))
        }

        # sleep to avoid overloading requests
        if (request_count % 5) == 0:
            print(request_count)
            time.sleep(100)


        # for each resort near the airport, make a send request
        if source_id_dict[str(destination)] == 'YYC':
           for item in calgary_list:
                send_request(json.dumps(data_object), item)
        elif source_id_dict[str(destination)] == 'YVR':
            for item in van_list:
                send_request(json.dumps(data_object), item)
        elif source_id_dict[str(destination)] == 'YLW':
            for item in kelowna_list:
                send_request(json.dumps(data_object), item)


        request_count += 1



