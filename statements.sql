-- newAccountCreated
INSERT INTO users (username, password) VALUES (?, ?)

-- isCorrectLogin
SELECT password FROM users WHERE username = ?

-- setLogin
UPDATE users SET logged_in = 1 WHERE id = ?

-- getLogInStatus
SELECT logged_in FROM users WHERE username = ?

-- getUserId
SELECT id FROM users WHERE username = ?

-- changeUsername
UPDATE users SET username = ? WHERE id = ?

-- changePassword
UPDATE users SET password = ? WHERE id = ?

-- logOut
UPDATE users SET logged_in = 0 WHERE id = ?

-- deleteAccount
DELETE FROM users WHERE id = ?

-- isUniqueUsername
SELECT count(username) FROM users WHERE username = ?

-- addSession
INSERT INTO sessions (user_id, client_fd) VALUES (?, ?)

-- removeSession
DELETE FROM sessions WHERE user_id = ?

-- addScore
INSERT INTO Scores (user_id, mode, score) VALUES (?, 'Endless', ?)

-- getTopScores
SELECT Users.username, MAX(Scores.score) FROM Scores
        JOIN Users ON Scores.user_id = Users.id
        WHERE Scores.mode = 'Endless'
        GROUP BY Users.username
        ORDER BY MAX(Scores.score) DESC
        LIMIT ?

-- getFriendsList
SELECT u.username FROM users u 
        JOIN friends r ON u.id = r.friend_id 
        WHERE r.user_id = (SELECT id FROM users WHERE username = ?)

-- deleteFriend
DELETE FROM friends WHERE 
    (user_id = (SELECT id FROM users WHERE username = ?) AND 
    friend_id = (SELECT id FROM users WHERE username = ?))
    OR 
    (user_id = (SELECT id FROM users WHERE username = ?) AND 
    friend_id = (SELECT id FROM users WHERE username = ?))

-- addFriend
INSERT INTO friends (user_id, friend_id) VALUES 
    ((SELECT id FROM users WHERE username = ?), 
    (SELECT id FROM users WHERE username = ?)), 

    ((SELECT id FROM users WHERE username = ?), 
    (SELECT id FROM users WHERE username = ?))

-- verifyFriendRequest
SELECT CASE 
    WHEN NOT EXISTS (SELECT 1 FROM users WHERE username = ?) THEN 1
    WHEN EXISTS (SELECT 1 FROM friends r 
                 JOIN users u1 ON u1.id = r.user_id 
                 JOIN users u2 ON u2.id = r.friend_id 
                 WHERE u1.username = ? AND u2.username = ?) THEN 2 
    WHEN EXISTS (SELECT 1 FROM friendsRequest fr 
                 JOIN users u1 ON u1.id = fr.user_id 
                 JOIN users u2 ON u2.id = fr.friend_id 
                 WHERE u1.username = ? AND u2.username = ?) THEN 3 
    WHEN EXISTS (SELECT 1 FROM friendsRequest fr
                 JOIN users u1 ON u1.id = fr.friend_id
                 JOIN users u2 ON u2.id = fr.user_id
                 WHERE u1.username = ? AND u2.username = ?) THEN 4
    ELSE 0 
    END

-- sendFriendRequest
INSERT INTO friendsRequest (user_id, friend_id) 
    VALUES ((SELECT id FROM users WHERE username = ?), 
            (SELECT id FROM users WHERE username = ?))

-- deleteFriendRequest
DELETE FROM friendsRequest WHERE
    (user_id = (SELECT id FROM users WHERE username = ?) AND 
    friend_id = (SELECT id FROM users WHERE username = ?))
    OR 
    (user_id = (SELECT id FROM users WHERE username = ?) AND
    friend_id = (SELECT id FROM users WHERE username = ?))

-- getNumberFriendRequestsReceived
SELECT COUNT(*) FROM friendsRequest
    WHERE friend_id = (SELECT id FROM users WHERE username = ?)

-- getReceivedRequestsList
SELECT u.username FROM users u
    JOIN friendsRequest r ON u.id = r.user_id
    WHERE r.friend_id = (SELECT id FROM users WHERE username = ?)

-- getSentRequestsList
SELECT u.username FROM users u
    JOIN friendsRequest r ON u.id = r.friend_id
    WHERE r.user_id = (SELECT id FROM users WHERE username = ?)

-- openConversation
UPDATE friends SET in_conversation =
    CASE WHEN in_conversation = 0 THEN 1
    ELSE in_conversation END
    WHERE (user_id = ? AND friend_id = ?)

-- closeConversation
UPDATE friends SET in_conversation =
    CASE WHEN in_conversation = 1 THEN 0
    ELSE in_conversation END
    WHERE (user_id = ? AND friend_id = ?)

-- getInConversation
SELECT in_conversation FROM friends WHERE (user_id = ? AND friend_id = ?)

-- addMessage
INSERT INTO MessageNoRead (sender_id, receiver_id, message) VALUES (?, ?, ?)

-- deleteMessages
DELETE FROM MessageNoRead WHERE sender_id = ? AND receiver_id = ?

-- getMessagesNoRead
SELECT message FROM MessageNoRead WHERE sender_id = ? AND receiver_id = ?

-- getNumberMessageNoRead
SELECT COUNT(*) FROM MessageNoRead WHERE sender_id = ? AND receiver_id = ?

-- setNotInConversation
UPDATE friends SET in_conversation = 0 WHERE user_id = ?

-- isUserInSession
SELECT 1 FROM sessions WHERE user_id = ? LIMIT 1

-- deleteSession
DELETE FROM sessions WHERE user_id = ?

-- getUserIdSession
SELECT user_id FROM sessions

-- getUserSocket
SELECT client_fd FROM sessions WHERE user_id = ?