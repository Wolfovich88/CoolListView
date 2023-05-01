#!/bin/python3

import sqlite3
import random
import string
import os
import argparse

parser = argparse.ArgumentParser(description='Set generated database file name')
parser.add_argument('filename')
args = parser.parse_args()
databaseName = args.filename

print("Generated database:" + databaseName)
dir_path = os.path.dirname(databaseName)

if dir_path:
    os.makedirs(dir_path, exist_ok=True)

if os.path.exists(databaseName):
    os.remove(databaseName)

# Connect to the database
conn = sqlite3.connect(databaseName)
c = conn.cursor()
             
# Check if the table exists
c.execute("SELECT name FROM sqlite_master WHERE type='table' AND name='entries'")
if c.fetchone() is None:
    # Create the table if it does not exist
    c.execute('''CREATE TABLE entries
             (id INTEGER PRIMARY KEY, name TEXT, message TEXT)''')
else:
    c.execute("DELETE FROM entries")

# Generate and insert 10,000 random entries
for i in range(10000):
    # Generate a random name
    name = ''.join(random.choices(string.ascii_lowercase, k=random.randint(5, 10)))
    
    # Generate a random message
    message = ''.join(random.choices(string.ascii_letters + string.digits + '.,?! ', k=random.randint(30, 100)))

    # Insert the entry into the table
    c.execute("INSERT INTO entries VALUES (?, ?, ?)", (i + 1, name, message)) #from 1 to 10k

# Commit the changes and close the connection
conn.commit()
conn.close()
