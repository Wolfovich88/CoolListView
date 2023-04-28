#!/bin/python3

import sqlite3
import random
import string

# Connect to the database
conn = sqlite3.connect('testdatabase.db')
c = conn.cursor()
             
# Check if the table exists
c.execute("SELECT name FROM sqlite_master WHERE type='table' AND name='entries'")
if c.fetchone() is None:
    # Create the table if it does not exist
    c.execute('''CREATE TABLE entries
             (name TEXT, message TEXT)''')
else:
    c.execute("DELETE FROM entries")

# Generate and insert 10,000 random entries
for i in range(10000):
    # Generate a random name
    name = ''.join(random.choices(string.ascii_lowercase, k=random.randint(5, 10)))
    
    # Generate a random message
    message = ''.join(random.choices(string.ascii_letters + string.digits + '.,?! ', k=random.randint(30, 100)))

    # Insert the entry into the table
    c.execute("INSERT INTO entries VALUES (?, ?)", (name, message))

# Commit the changes and close the connection
conn.commit()
conn.close()
