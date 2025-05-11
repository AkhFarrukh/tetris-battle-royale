-- createUsersTable
CREATE TABLE IF NOT EXISTS users (
    id INTEGER PRIMARY KEY,
    username TEXT UNIQUE,
    password TEXT,
    logged_in INTEGER DEFAULT 0,
    receiving_file_descriptor INTEGER DEFAULT 0, 
    sending_file_descriptor INTEGER DEFAULT 0
);
-- createFriendsTable
CREATE TABLE IF NOT EXISTS friends (
    user_id INTEGER,
    friend_id INTEGER,
    in_conversation INTEGER DEFAULT 0,
    FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE,
    FOREIGN KEY(friend_id) REFERENCES users(id) ON DELETE CASCADE,
    PRIMARY KEY (user_id, friend_id)
);
-- createFriendsRequestTable
CREATE TABLE IF NOT EXISTS friendsRequest (
    user_id INTEGER,
    friend_id INTEGER,
    FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE,
    FOREIGN KEY(friend_id) REFERENCES users(id) ON DELETE CASCADE,
    PRIMARY KEY (user_id, friend_id)
);
-- createSessionsTable
CREATE TABLE IF NOT EXISTS sessions (
    user_id INTEGER,
    client_fd INTEGER,
    FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE,
    PRIMARY KEY (user_id)
);
-- createMessageNoReadTable
CREATE TABLE IF NOT EXISTS MessageNoRead (
    message_id INTEGER PRIMARY KEY AUTOINCREMENT,
    sender_id INTEGER,
    receiver_id INTEGER,
    message TEXT,
    FOREIGN KEY(sender_id) REFERENCES users(id) ON DELETE CASCADE,
    FOREIGN KEY(receiver_id) REFERENCES users(id) ON DELETE CASCADE
);
-- createScoresTable
CREATE TABLE IF NOT EXISTS Scores (
                        id INTEGER PRIMARY KEY AUTOINCREMENT,
                        user_id INTEGER NOT NULL,
                        mode TEXT NOT NULL DEFAULT 'Endless' CHECK(mode IN ('Endless')),
                        score INTEGER NOT NULL,
                        timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
                        FOREIGN KEY(user_id) REFERENCES Users(id) ON DELETE CASCADE
);