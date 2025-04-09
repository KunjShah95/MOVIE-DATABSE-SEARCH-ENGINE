#include <iostream>
#include <cstring> // For basic string operations
#include <fstream> // For file operations
#include <cstdio>  // For file system operations

// Maximum sizes for arrays
const int MAX_MOVIES = 50;
const int MAX_USERS = 20;
const int MAX_CAST = 5;
const int MAX_STRING_LENGTH = 100;
const char *DB_FILENAME = "movies_database.dat";

// Movie class definition
class Movie
{
public:
    char title[MAX_STRING_LENGTH];
    int releaseYear;
    char director[MAX_STRING_LENGTH];
    char cast[MAX_CAST][MAX_STRING_LENGTH];
    int castCount;
    char genre[MAX_STRING_LENGTH];
    float rating;
    int duration;

    // Constructor
    Movie()
    {
        title[0] = '\0';
        releaseYear = 0;
        director[0] = '\0';
        castCount = 0;
        genre[0] = '\0';
        rating = 0.0f;
        duration = 0;
    }

    // Initialize movie with data
    void initialize(const char *t, int year, const char *dir,
                    const char castList[][MAX_STRING_LENGTH], int castSize,
                    const char *gen, float r, int dur)
    {
        strcpy(title, t);
        releaseYear = year;
        strcpy(director, dir);

        castCount = (castSize > MAX_CAST) ? MAX_CAST : castSize;
        for (int i = 0; i < castCount; i++)
        {
            strcpy(cast[i], castList[i]);
        }

        strcpy(genre, gen);
        rating = r;
        duration = dur;
    }

    // Function to display movie details
    void display() const
    {
        std::cout << "Title: " << title << std::endl;
        std::cout << "Release Year: " << releaseYear << std::endl;
        std::cout << "Director: " << director << std::endl;
        std::cout << "Cast: ";
        for (int i = 0; i < castCount; i++)
        {
            std::cout << cast[i] << " ";
        }
        std::cout << std::endl;
        std::cout << "Genre: " << genre << std::endl;
        std::cout << "Rating: " << rating << std::endl;
        std::cout << "Duration: " << duration << " minutes" << std::endl;
        std::cout << "--------------------------------------" << std::endl;
    }
};

// User class for storing user ratings
class User
{
public:
    int userId;
    char username[MAX_STRING_LENGTH];

    // Ratings storage
    struct Rating
    {
        char movieTitle[MAX_STRING_LENGTH];
        float rating;
        bool used;
    };
    Rating ratings[MAX_MOVIES];
    int ratingCount;

    // Constructor
    User()
    {
        userId = 0;
        username[0] = '\0';
        ratingCount = 0;
        for (int i = 0; i < MAX_MOVIES; i++)
        {
            ratings[i].used = false;
        }
    }

    // Initialize user
    void initialize(int id, const char *name)
    {
        userId = id;
        strcpy(username, name);
    }

    // Rate a movie
    void rateMovie(const char *movieTitle, float rating)
    {
        if (rating >= 0 && rating <= 10)
        {
            // Check if movie is already rated
            for (int i = 0; i < MAX_MOVIES; i++)
            {
                if (ratings[i].used && strcmp(ratings[i].movieTitle, movieTitle) == 0)
                {
                    ratings[i].rating = rating;
                    std::cout << "Rating updated successfully!" << std::endl;
                    return;
                }
            }

            // Find empty slot for new rating
            if (ratingCount < MAX_MOVIES)
            {
                int index = 0;
                while (index < MAX_MOVIES && ratings[index].used)
                {
                    index++;
                }

                if (index < MAX_MOVIES)
                {
                    strcpy(ratings[index].movieTitle, movieTitle);
                    ratings[index].rating = rating;
                    ratings[index].used = true;
                    ratingCount++;
                    std::cout << "Rating added successfully!" << std::endl;
                }
                else
                {
                    std::cout << "Error: No more space for ratings." << std::endl;
                }
            }
            else
            {
                std::cout << "Error: Maximum ratings reached." << std::endl;
            }
        }
        else
        {
            std::cout << "Invalid rating! Please enter a rating between 0 and 10." << std::endl;
        }
    }

    // Check if user has rated a movie
    bool hasRated(const char *movieTitle) const
    {
        for (int i = 0; i < MAX_MOVIES; i++)
        {
            if (ratings[i].used && strcmp(ratings[i].movieTitle, movieTitle) == 0)
            {
                return true;
            }
        }
        return false;
    }

    // Get rating for a movie
    float getRating(const char *movieTitle) const
    {
        for (int i = 0; i < MAX_MOVIES; i++)
        {
            if (ratings[i].used && strcmp(ratings[i].movieTitle, movieTitle) == 0)
            {
                return ratings[i].rating;
            }
        }
        return -1.0f; // Not found
    }

    // Display all ratings by this user
    void displayRatings() const
    {
        if (ratingCount == 0)
        {
            std::cout << "No ratings yet." << std::endl;
            return;
        }

        std::cout << "Ratings by " << username << ":" << std::endl;
        for (int i = 0; i < MAX_MOVIES; i++)
        {
            if (ratings[i].used)
            {
                std::cout << ratings[i].movieTitle << ": " << ratings[i].rating << "/10" << std::endl;
            }
        }
    }
};

// Database class to manage movies and users
class MovieDatabase
{
private:
    Movie movies[MAX_MOVIES];
    int movieCount;

    User users[MAX_USERS];
    int userCount;
    int currentUserId;

public:
    MovieDatabase() : movieCount(0), userCount(0), currentUserId(-1) {}

    // Save database to file
    bool saveToFile(const char *filename = DB_FILENAME)
    {
        std::cout << "Attempting to save database to " << filename << std::endl;
        std::cout << "Current movies: " << movieCount << ", Current users: " << userCount << std::endl;

        FILE *fp = fopen(filename, "wb");
        if (!fp)
        {
            std::cout << "Error: Could not open file for writing! Make sure you have write permissions." << std::endl;
            perror("File open error");
            return false;
        }

        // Write file signature
        const char signature[8] = "MVDB100";
        size_t sigWritten = fwrite(signature, sizeof(char), 8, fp);
        if (sigWritten != 8)
        {
            std::cout << "Error: Failed to write file signature." << std::endl;
            perror("Write error");
            fclose(fp);
            return false;
        }

        // Write movie count
        size_t countWritten = fwrite(&movieCount, sizeof(int), 1, fp);
        if (countWritten != 1)
        {
            std::cout << "Error: Failed to write movie count." << std::endl;
            perror("Write error");
            fclose(fp);
            return false;
        }

        // Write each movie
        for (int i = 0; i < movieCount; i++)
        {
            size_t movieWritten = fwrite(&movies[i], sizeof(Movie), 1, fp);
            if (movieWritten != 1)
            {
                std::cout << "Error: Failed to write movie data for movie " << i << "." << std::endl;
                perror("Write error");
                fclose(fp);
                return false;
            }
        }

        // Write user count
        size_t userCountWritten = fwrite(&userCount, sizeof(int), 1, fp);
        if (userCountWritten != 1)
        {
            std::cout << "Error: Failed to write user count." << std::endl;
            perror("Write error");
            fclose(fp);
            return false;
        }

        // Write each user
        for (int i = 0; i < userCount; i++)
        {
            size_t userWritten = fwrite(&users[i], sizeof(User), 1, fp);
            if (userWritten != 1)
            {
                std::cout << "Error: Failed to write user data for user " << i << "." << std::endl;
                perror("Write error");
                fclose(fp);
                return false;
            }
        }

        // Flush and close the file
        fflush(fp);
        fclose(fp);

        // Verify file was created with non-zero size
        FILE *check = fopen(filename, "rb");
        if (!check)
        {
            std::cout << "Warning: Could not verify database file was created." << std::endl;
            return false;
        }

        // Get file size
        fseek(check, 0, SEEK_END);
        long fileSize = ftell(check);
        fclose(check);

        if (fileSize > 0)
        {
            std::cout << "Database saved successfully to " << filename << " (" << fileSize << " bytes)" << std::endl;
            return true;
        }
        else
        {
            std::cout << "Error: Database file was created but appears to be empty!" << std::endl;
            return false;
        }
    }

    // Load database from file
    bool loadFromFile(const char *filename = DB_FILENAME)
    {
        FILE *fp = fopen(filename, "rb");
        if (!fp)
        {
            std::cout << "Warning: Could not open database file for reading. Creating a new database." << std::endl;
            return false;
        }

        // Read and verify file signature
        char signature[8];
        if (fread(signature, sizeof(char), 8, fp) != 8 || strncmp(signature, "MVDB100", 7) != 0)
        {
            std::cout << "Error: Invalid database file format. Creating a new database." << std::endl;
            fclose(fp);
            return false;
        }

        // Read movie count
        if (fread(&movieCount, sizeof(int), 1, fp) != 1)
        {
            std::cout << "Error: Failed to read movie count. Creating a new database." << std::endl;
            fclose(fp);
            return false;
        }

        if (movieCount < 0 || movieCount > MAX_MOVIES)
        {
            std::cout << "Error: Corrupted database file (invalid movie count). Creating a new database." << std::endl;
            fclose(fp);
            movieCount = 0;
            return false;
        }

        // Read each movie
        for (int i = 0; i < movieCount; i++)
        {
            if (fread(&movies[i], sizeof(Movie), 1, fp) != 1)
            {
                std::cout << "Error: Failed to read movie data. Creating a new database." << std::endl;
                fclose(fp);
                movieCount = 0;
                return false;
            }
        }

        // Read user count
        if (fread(&userCount, sizeof(int), 1, fp) != 1)
        {
            std::cout << "Error: Failed to read user count. Creating a new database." << std::endl;
            fclose(fp);
            movieCount = 0;
            userCount = 0;
            return false;
        }

        if (userCount < 0 || userCount > MAX_USERS)
        {
            std::cout << "Error: Corrupted database file (invalid user count). Creating a new database." << std::endl;
            fclose(fp);
            movieCount = 0;
            userCount = 0;
            return false;
        }

        // Read each user
        for (int i = 0; i < userCount; i++)
        {
            if (fread(&users[i], sizeof(User), 1, fp) != 1)
            {
                std::cout << "Error: Failed to read user data. Creating a new database." << std::endl;
                fclose(fp);
                movieCount = 0;
                userCount = 0;
                return false;
            }
        }

        fclose(fp);
        std::cout << "Database loaded successfully from " << filename << std::endl;
        std::cout << "Loaded " << movieCount << " movies and " << userCount << " users." << std::endl;
        return true;
    }

    // Add a movie to the database
    void addMovie(const Movie &movie)
    {
        if (movieCount < MAX_MOVIES)
        {
            movies[movieCount++] = movie;
            std::cout << "Movie added successfully!" << std::endl;
        }
        else
        {
            std::cout << "Error: Database is full." << std::endl;
        }
    }

    // Add a user to the database
    void addUser(const User &user)
    {
        if (userCount < MAX_USERS)
        {
            users[userCount++] = user;
            std::cout << "User added successfully!" << std::endl;
        }
        else
        {
            std::cout << "Error: Maximum users reached." << std::endl;
        }
    }

    // Get movie by title
    Movie *getMovieByTitle(const char *title)
    {
        for (int i = 0; i < movieCount; i++)
        {
            if (strcmp(movies[i].title, title) == 0)
            {
                return &movies[i];
            }
        }
        return nullptr;
    }

    // Linear search movies by partial title
    void searchByTitle(const char *title)
    {
        bool found = false;
        for (int i = 0; i < movieCount; i++)
        {
            // Basic substring search
            const char *result = strstr(movies[i].title, title);
            if (result != nullptr)
            {
                movies[i].display();
                found = true;
            }
        }
        if (!found)
        {
            std::cout << "No movies found with title: " << title << std::endl;
        }
    }

    // Search by exact year
    void searchByYear(int year)
    {
        bool found = false;
        for (int i = 0; i < movieCount; i++)
        {
            if (movies[i].releaseYear == year)
            {
                movies[i].display();
                found = true;
            }
        }
        if (!found)
        {
            std::cout << "No movies found with release year: " << year << std::endl;
        }
    }

    // Search by genre
    void searchByGenre(const char *genre)
    {
        bool found = false;
        for (int i = 0; i < movieCount; i++)
        {
            if (strcmp(movies[i].genre, genre) == 0)
            {
                movies[i].display();
                found = true;
            }
        }
        if (!found)
        {
            std::cout << "No movies found with genre: " << genre << std::endl;
        }
    }

    // Search by director
    void searchByDirector(const char *director)
    {
        bool found = false;
        for (int i = 0; i < movieCount; i++)
        {
            if (strcmp(movies[i].director, director) == 0)
            {
                movies[i].display();
                found = true;
            }
        }
        if (!found)
        {
            std::cout << "No movies found with director: " << director << std::endl;
        }
    }

    // Delete a movie
    void deleteMovie(const char *title)
    {
        for (int i = 0; i < movieCount; i++)
        {
            if (strcmp(movies[i].title, title) == 0)
            {
                // Shift remaining movies
                for (int j = i; j < movieCount - 1; j++)
                {
                    movies[j] = movies[j + 1];
                }
                movieCount--;
                std::cout << "Movie deleted successfully!" << std::endl;
                return;
            }
        }
        std::cout << "Movie not found!" << std::endl;
    }

    // View all movies
    void viewAllMovies()
    {
        if (movieCount == 0)
        {
            std::cout << "No movies in the database." << std::endl;
            return;
        }

        for (int i = 0; i < movieCount; i++)
        {
            movies[i].display();
        }
    }

    // Sort movies by rating using bubble sort
    void sortByRatingBubble()
    {
        for (int i = 0; i < movieCount; i++)
        {
            for (int j = 0; j < movieCount - i - 1; j++)
            {
                if (movies[j].rating < movies[j + 1].rating)
                {
                    // Swap movies
                    Movie temp = movies[j];
                    movies[j] = movies[j + 1];
                    movies[j + 1] = temp;
                }
            }
        }
        std::cout << "Movies sorted by rating (Bubble Sort)!" << std::endl;
    }

    // Sort movies by rating using selection sort
    void sortByRatingSelection()
    {
        for (int i = 0; i < movieCount; i++)
        {
            int maxIndex = i;
            for (int j = i + 1; j < movieCount; j++)
            {
                if (movies[j].rating > movies[maxIndex].rating)
                {
                    maxIndex = j;
                }
            }
            // Swap movies
            if (maxIndex != i)
            {
                Movie temp = movies[i];
                movies[i] = movies[maxIndex];
                movies[maxIndex] = temp;
            }
        }
        std::cout << "Movies sorted by rating (Selection Sort)!" << std::endl;
    }

    // Login as a user
    bool loginUser(int userId)
    {
        for (int i = 0; i < userCount; i++)
        {
            if (users[i].userId == userId)
            {
                currentUserId = userId;
                std::cout << "Logged in as " << users[i].username << std::endl;
                return true;
            }
        }
        std::cout << "User not found!" << std::endl;
        return false;
    }

    // Create a new user
    void createUser(const char *username)
    {
        if (userCount < MAX_USERS)
        {
            int newId = userCount + 1;
            User newUser;
            newUser.initialize(newId, username);
            users[userCount++] = newUser;
            currentUserId = newId;
            std::cout << "Created user " << username << " with ID " << newId << std::endl;
        }
        else
        {
            std::cout << "Error: Maximum users reached." << std::endl;
        }
    }

    // Rate a movie
    void rateMovie(const char *title, float rating)
    {
        if (currentUserId == -1)
        {
            std::cout << "Please login first!" << std::endl;
            return;
        }

        // Find the movie
        bool movieFound = false;
        for (int i = 0; i < movieCount; i++)
        {
            if (strcmp(movies[i].title, title) == 0)
            {
                movieFound = true;
                break;
            }
        }

        if (!movieFound)
        {
            std::cout << "Movie not found!" << std::endl;
            return;
        }

        // Find the user and add rating
        for (int i = 0; i < userCount; i++)
        {
            if (users[i].userId == currentUserId)
            {
                users[i].rateMovie(title, rating);
                return;
            }
        }
    }

    // Display user ratings
    void displayUserRatings()
    {
        if (currentUserId == -1)
        {
            std::cout << "Please login first!" << std::endl;
            return;
        }

        for (int i = 0; i < userCount; i++)
        {
            if (users[i].userId == currentUserId)
            {
                users[i].displayRatings();
                return;
            }
        }
    }

    // Calculate similarity between two movies
    float calculateMovieSimilarity(const Movie &movie1, const Movie &movie2)
    {
        float similarity = 0.0f;

        // Same genre is a strong indicator
        if (strcmp(movie1.genre, movie2.genre) == 0)
        {
            similarity += 3.0f;
        }

        // Same director is also significant
        if (strcmp(movie1.director, movie2.director) == 0)
        {
            similarity += 2.0f;
        }

        // Rating similarity (inverse of difference)
        similarity += (10.0f - abs(movie1.rating - movie2.rating)) * 0.5f;

        // Release year similarity (closer years get higher scores)
        float yearDiff = abs(movie1.releaseYear - movie2.releaseYear) / 10.0f;
        similarity += (5.0f - (yearDiff > 5.0f ? 5.0f : yearDiff)) * 0.2f;

        return similarity;
    }

    // Find similar movies
    void findSimilarMovies(const char *title)
    {
        Movie *targetMovie = getMovieByTitle(title);
        if (targetMovie == nullptr)
        {
            std::cout << "Movie not found!" << std::endl;
            return;
        }

        // Store similarity scores
        struct MovieSimilarity
        {
            int movieIndex;
            float score;
        };
        MovieSimilarity similarities[MAX_MOVIES];
        int simCount = 0;

        // Calculate similarities
        for (int i = 0; i < movieCount; i++)
        {
            if (strcmp(movies[i].title, title) != 0)
            { // Skip the target movie
                similarities[simCount].movieIndex = i;
                similarities[simCount].score = calculateMovieSimilarity(*targetMovie, movies[i]);
                simCount++;
            }
        }

        // Sort by similarity (bubble sort)
        for (int i = 0; i < simCount; i++)
        {
            for (int j = 0; j < simCount - i - 1; j++)
            {
                if (similarities[j].score < similarities[j + 1].score)
                {
                    // Swap
                    MovieSimilarity temp = similarities[j];
                    similarities[j] = similarities[j + 1];
                    similarities[j + 1] = temp;
                }
            }
        }

        // Display the top 5 similar movies
        std::cout << "Similar movies to " << title << ":" << std::endl;
        int limit = (simCount < 5) ? simCount : 5;
        for (int i = 0; i < limit; i++)
        {
            movies[similarities[i].movieIndex].display();
        }
    }

    // Get recommendations based on user's highest rated movie
    void getRecommendations()
    {
        if (currentUserId == -1)
        {
            std::cout << "Please login first!" << std::endl;
            return;
        }

        // Find the current user
        User *currentUser = nullptr;
        for (int i = 0; i < userCount; i++)
        {
            if (users[i].userId == currentUserId)
            {
                currentUser = &users[i];
                break;
            }
        }

        if (currentUser == nullptr || currentUser->ratingCount == 0)
        {
            std::cout << "Please rate some movies first to get recommendations." << std::endl;
            return;
        }

        std::cout << "\n--- Recommendations for " << currentUser->username << " ---" << std::endl;

        // Find highest rated movie
        char highestRatedMovie[MAX_STRING_LENGTH] = "";
        float highestRating = 0;

        for (int i = 0; i < MAX_MOVIES; i++)
        {
            if (currentUser->ratings[i].used && currentUser->ratings[i].rating > highestRating)
            {
                highestRating = currentUser->ratings[i].rating;
                strcpy(highestRatedMovie, currentUser->ratings[i].movieTitle);
            }
        }

        // Find similar movies to the highest rated
        if (highestRatedMovie[0] != '\0')
        {
            std::cout << "Based on your highest rated movie (" << highestRatedMovie << "):" << std::endl;
            findSimilarMovies(highestRatedMovie);
        }
    }

    // Modified runMenu to handle database saving
    void runMenu()
    {
        // If no data loaded from file, add sample users
        if (userCount == 0)
        {
            User user1;
            user1.initialize(1, "Alice");
            addUser(user1);

            User user2;
            user2.initialize(2, "Bob");
            addUser(user2);

            User user3;
            user3.initialize(3, "Charlie");
            addUser(user3);
        }

        int choice;
        do
        {
            std::cout << "\nMovie Database Search System\n";
            std::cout << "1. Search by Title\n";
            std::cout << "2. Search by Release Year\n";
            std::cout << "3. Add Movie\n";
            std::cout << "4. Delete Movie\n";
            std::cout << "5. View All Movies\n";
            std::cout << "6. Search by Genre\n";
            std::cout << "7. Search by Director\n";
            std::cout << "8. Sort by Rating (Bubble Sort)\n";
            std::cout << "9. Sort by Rating (Selection Sort)\n";
            std::cout << "10. User Login/Switch\n";
            std::cout << "11. Rate a Movie\n";
            std::cout << "12. View My Ratings\n";
            std::cout << "13. Get Movie Recommendations\n";
            std::cout << "14. Find Similar Movies\n";
            std::cout << "15. Save Database\n"; // New option
            std::cout << "16. Exit\n";          // Changed to 16
            std::cout << "Enter your choice: ";
            std::cin >> choice;
            std::cin.ignore(); // Ignore the newline character left in the input buffer

            // Display current user if logged in
            if (currentUserId != -1)
            {
                for (int i = 0; i < userCount; i++)
                {
                    if (users[i].userId == currentUserId)
                    {
                        std::cout << "[Currently logged in as: " << users[i].username << "]" << std::endl;
                        break;
                    }
                }
            }

            switch (choice)
            {
            case 1:
            {
                char title[MAX_STRING_LENGTH];
                std::cout << "Enter movie title: ";
                std::cin.getline(title, MAX_STRING_LENGTH);
                searchByTitle(title);
                break;
            }
            case 2:
            {
                int year;
                std::cout << "Enter release year: ";
                std::cin >> year;
                searchByYear(year);
                break;
            }
            case 3:
            {
                Movie movie;
                char title[MAX_STRING_LENGTH];
                int releaseYear;
                char director[MAX_STRING_LENGTH];
                char genre[MAX_STRING_LENGTH];
                float rating;
                int duration;

                std::cout << "Enter movie title: ";
                std::cin.ignore();
                std::cin.getline(title, MAX_STRING_LENGTH);

                std::cout << "Enter release year: ";
                std::cin >> releaseYear;

                std::cout << "Enter director: ";
                std::cin.ignore();
                std::cin.getline(director, MAX_STRING_LENGTH);

                std::cout << "Enter genre: ";
                std::cin.getline(genre, MAX_STRING_LENGTH);

                std::cout << "Enter rating: ";
                std::cin >> rating;

                std::cout << "Enter duration (in minutes): ";
                std::cin >> duration;

                int castCount;
                std::cout << "Enter number of cast members (max 5): ";
                std::cin >> castCount;
                if (castCount > MAX_CAST)
                    castCount = MAX_CAST;
                std::cin.ignore();

                char castList[MAX_CAST][MAX_STRING_LENGTH];
                for (int i = 0; i < castCount; i++)
                {
                    std::cout << "Enter cast member " << i + 1 << ": ";
                    std::cin.getline(castList[i], MAX_STRING_LENGTH);
                }

                movie.initialize(title, releaseYear, director, castList, castCount, genre, rating, duration);
                addMovie(movie);
                break;
            }
            case 4:
            {
                char title[MAX_STRING_LENGTH];
                std::cout << "Enter movie title to delete: ";
                std::cin.getline(title, MAX_STRING_LENGTH);
                deleteMovie(title);
                break;
            }
            case 5:
                viewAllMovies();
                break;
            case 6:
            {
                char genre[MAX_STRING_LENGTH];
                std::cout << "Enter genre: ";
                std::cin.getline(genre, MAX_STRING_LENGTH);
                searchByGenre(genre);
                break;
            }
            case 7:
            {
                char director[MAX_STRING_LENGTH];
                std::cout << "Enter director: ";
                std::cin.getline(director, MAX_STRING_LENGTH);
                searchByDirector(director);
                break;
            }
            case 8:
                sortByRatingBubble();
                viewAllMovies();
                break;
            case 9:
                sortByRatingSelection();
                viewAllMovies();
                break;
            case 10:
            {
                int option;
                std::cout << "1. Login as existing user\n";
                std::cout << "2. Create new user\n";
                std::cout << "Enter option: ";
                std::cin >> option;

                if (option == 1)
                {
                    int userId;
                    std::cout << "Available users:" << std::endl;
                    for (int i = 0; i < userCount; i++)
                    {
                        std::cout << users[i].userId << ": " << users[i].username << std::endl;
                    }
                    std::cout << "Enter user ID: ";
                    std::cin >> userId;
                    loginUser(userId);
                }
                else if (option == 2)
                {
                    char username[MAX_STRING_LENGTH];
                    std::cout << "Enter username: ";
                    std::cin.ignore();
                    std::cin.getline(username, MAX_STRING_LENGTH);
                    createUser(username);
                }
                break;
            }
            case 11:
            {
                if (currentUserId == -1)
                {
                    std::cout << "Please login first!" << std::endl;
                    break;
                }

                char title[MAX_STRING_LENGTH];
                float rating;

                std::cout << "Available movies:" << std::endl;
                for (int i = 0; i < movieCount; i++)
                {
                    std::cout << "- " << movies[i].title << std::endl;
                }

                std::cout << "Enter movie title: ";
                std::cin.getline(title, MAX_STRING_LENGTH);

                std::cout << "Enter rating (0-10): ";
                std::cin >> rating;

                rateMovie(title, rating);
                break;
            }
            case 12:
                displayUserRatings();
                break;
            case 13:
                getRecommendations();
                break;
            case 14:
            {
                char title[MAX_STRING_LENGTH];
                std::cout << "Enter movie title to find similar movies: ";
                std::cin.getline(title, MAX_STRING_LENGTH);
                findSimilarMovies(title);
                break;
            }
            case 15:
                // Save database to file
                saveToFile();
                break;

            case 16:
                // Save database before exiting
                std::cout << "Saving database before exiting..." << std::endl;
                saveToFile();
                std::cout << "Exiting..." << std::endl;
                break;

            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
            }
        } while (choice != 16); // Changed to 16
    }

    // Initialize the database with sample data
    void initializeWithSampleData()
    {
        // Sample data for The Shawshank Redemption
        char cast1[MAX_CAST][MAX_STRING_LENGTH] = {"Tim Robbins", "Morgan Freeman", "Bob Gunton"};
        Movie movie1;
        movie1.initialize("The Shawshank Redemption", 1994, "Frank Darabont", cast1, 3, "Drama", 9.3f, 142);
        addMovie(movie1);

        // Sample data for The Godfather
        char cast2[MAX_CAST][MAX_STRING_LENGTH] = {"Marlon Brando", "Al Pacino", "James Caan"};
        Movie movie2;
        movie2.initialize("The Godfather", 1972, "Francis Ford Coppola", cast2, 3, "Crime", 9.2f, 175);
        addMovie(movie2);

        // Sample data for The Dark Knight
        char cast3[MAX_CAST][MAX_STRING_LENGTH] = {"Christian Bale", "Heath Ledger", "Aaron Eckhart"};
        Movie movie3;
        movie3.initialize("The Dark Knight", 2008, "Christopher Nolan", cast3, 3, "Action", 8.9f, 152);
        addMovie(movie3);

        // Sample data for Pulp Fiction
        char cast4[MAX_CAST][MAX_STRING_LENGTH] = {"John Travolta", "Uma Thurman", "Samuel L. Jackson"};
        Movie movie4;
        movie4.initialize("Pulp Fiction", 1994, "Quentin Tarantino", cast4, 3, "Crime", 8.5f, 154);
        addMovie(movie4);

        // Sample data for Inception
        char cast5[MAX_CAST][MAX_STRING_LENGTH] = {"Leonardo DiCaprio", "Joseph Gordon-Levitt", "Elliot Page"};
        Movie movie5;
        movie5.initialize("Inception", 2010, "Christopher Nolan", cast5, 3, "Sci-Fi", 8.8f, 148);
        addMovie(movie5);

        // Add recent English and Hindi movies (2020-2025)
        addRecentMovies();
    }

    // Function to add recent English and Hindi movies (2020-2025)
    void addRecentMovies()
    {
        // English Movies (2020-2025)

        // Dune (2021)
        char castDune[MAX_CAST][MAX_STRING_LENGTH] = {"Timothee Chalamet", "Rebecca Ferguson", "Zendaya", "Oscar Isaac", "Jason Momoa"};
        Movie dune;
        dune.initialize("Dune", 2021, "Denis Villeneuve", castDune, 5, "Sci-Fi", 8.0f, 155);
        addMovie(dune);

        // No Time To Die (2021)
        char castBond[MAX_CAST][MAX_STRING_LENGTH] = {"Daniel Craig", "Lea Seydoux", "Rami Malek", "Lashana Lynch", "Ana de Armas"};
        Movie noTimeToDie;
        noTimeToDie.initialize("No Time To Die", 2021, "Cary Joji Fukunaga", castBond, 5, "Action", 7.3f, 163);
        addMovie(noTimeToDie);

        // Oppenheimer (2023)
        char castOppenheimer[MAX_CAST][MAX_STRING_LENGTH] = {"Cillian Murphy", "Emily Blunt", "Matt Damon", "Robert Downey Jr.", "Florence Pugh"};
        Movie oppenheimer;
        oppenheimer.initialize("Oppenheimer", 2023, "Christopher Nolan", castOppenheimer, 5, "Drama", 8.4f, 180);
        addMovie(oppenheimer);

        // Barbie (2023)
        char castBarbie[MAX_CAST][MAX_STRING_LENGTH] = {"Margot Robbie", "Ryan Gosling", "America Ferrera", "Kate McKinnon", "Issa Rae"};
        Movie barbie;
        barbie.initialize("Barbie", 2023, "Greta Gerwig", castBarbie, 5, "Comedy", 7.0f, 114);
        addMovie(barbie);

        // Inside Out 2 (2024)
        char castInsideOut[MAX_CAST][MAX_STRING_LENGTH] = {"Amy Poehler", "Maya Hawke", "Kensington Tallman", "Liza Lapira", "Lewis Black"};
        Movie insideOut2;
        insideOut2.initialize("Inside Out 2", 2024, "Kelsey Mann", castInsideOut, 5, "Animation", 7.9f, 96);
        addMovie(insideOut2);

        // Tenet (2020)
        char castTenet[MAX_CAST][MAX_STRING_LENGTH] = {"John David Washington", "Robert Pattinson", "Elizabeth Debicki", "Kenneth Branagh", "Dimple Kapadia"};
        Movie tenet;
        tenet.initialize("Tenet", 2020, "Christopher Nolan", castTenet, 5, "Sci-Fi", 7.3f, 150);
        addMovie(tenet);

        // The Batman (2022)
        char castBatman[MAX_CAST][MAX_STRING_LENGTH] = {"Robert Pattinson", "Zoe Kravitz", "Paul Dano", "Jeffrey Wright", "Colin Farrell"};
        Movie theBatman;
        theBatman.initialize("The Batman", 2022, "Matt Reeves", castBatman, 5, "Action", 7.8f, 176);
        addMovie(theBatman);

        // Everything Everywhere All at Once (2022)
        char castEverything[MAX_CAST][MAX_STRING_LENGTH] = {"Michelle Yeoh", "Ke Huy Quan", "Stephanie Hsu", "Jamie Lee Curtis", "James Hong"};
        Movie everything;
        everything.initialize("Everything Everywhere All at Once", 2022, "Daniel Kwan, Daniel Scheinert", castEverything, 5, "Sci-Fi", 7.8f, 139);
        addMovie(everything);

        // Killers of the Flower Moon (2023)
        char castKillers[MAX_CAST][MAX_STRING_LENGTH] = {"Leonardo DiCaprio", "Robert De Niro", "Lily Gladstone", "Jesse Plemons", "Tantoo Cardinal"};
        Movie killers;
        killers.initialize("Killers of the Flower Moon", 2023, "Martin Scorsese", castKillers, 5, "Crime", 7.7f, 206);
        addMovie(killers);

        // Dune: Part Two (2024)
        char castDune2[MAX_CAST][MAX_STRING_LENGTH] = {"Timothee Chalamet", "Zendaya", "Rebecca Ferguson", "Austin Butler", "Florence Pugh"};
        Movie dune2;
        dune2.initialize("Dune: Part Two", 2024, "Denis Villeneuve", castDune2, 5, "Sci-Fi", 8.6f, 166);
        addMovie(dune2);

        // Hindi Movies (2020-2025)

        // Pathaan (2023)
        char castPathaan[MAX_CAST][MAX_STRING_LENGTH] = {"Shah Rukh Khan", "Deepika Padukone", "John Abraham", "Dimple Kapadia", "Ashutosh Rana"};
        Movie pathaan;
        pathaan.initialize("Pathaan", 2023, "Siddharth Anand", castPathaan, 5, "Action", 5.9f, 146);
        addMovie(pathaan);

        // Brahmastra: Part One (2022)
        char castBrahmastra[MAX_CAST][MAX_STRING_LENGTH] = {"Ranbir Kapoor", "Alia Bhatt", "Amitabh Bachchan", "Nagarjuna Akkineni", "Mouni Roy"};
        Movie brahmastra;
        brahmastra.initialize("Brahmastra: Part One", 2022, "Ayan Mukerji", castBrahmastra, 5, "Fantasy", 5.5f, 167);
        addMovie(brahmastra);

        // RRR (2022)
        char castRRR[MAX_CAST][MAX_STRING_LENGTH] = {"N.T. Rama Rao Jr.", "Ram Charan", "Ajay Devgn", "Alia Bhatt", "Shriya Saran"};
        Movie rrr;
        rrr.initialize("RRR", 2022, "S.S. Rajamouli", castRRR, 5, "Action", 7.8f, 187);
        addMovie(rrr);

        // Animal (2023)
        char castAnimal[MAX_CAST][MAX_STRING_LENGTH] = {"Ranbir Kapoor", "Anil Kapoor", "Bobby Deol", "Rashmika Mandanna", "Tripti Dimri"};
        Movie animal;
        animal.initialize("Animal", 2023, "Sandeep Reddy Vanga", castAnimal, 5, "Crime", 6.1f, 201);
        addMovie(animal);

        // Jawan (2023)
        char castJawan[MAX_CAST][MAX_STRING_LENGTH] = {"Shah Rukh Khan", "Nayanthara", "Vijay Sethupathi", "Deepika Padukone", "Priyamani"};
        Movie jawan;
        jawan.initialize("Jawan", 2023, "Atlee Kumar", castJawan, 5, "Action", 6.4f, 169);
        addMovie(jawan);

        // Bhool Bhulaiyaa 2 (2022)
        char castBhool[MAX_CAST][MAX_STRING_LENGTH] = {"Kartik Aaryan", "Kiara Advani", "Tabu", "Rajpal Yadav", "Sanjay Mishra"};
        Movie bhool;
        bhool.initialize("Bhool Bhulaiyaa 2", 2022, "Anees Bazmee", castBhool, 5, "Comedy", 5.7f, 143);
        addMovie(bhool);

        // Gangubai Kathiawadi (2022)
        char castGangubai[MAX_CAST][MAX_STRING_LENGTH] = {"Alia Bhatt", "Shantanu Maheshwari", "Vijay Raaz", "Indira Tiwari", "Seema Pahwa"};
        Movie gangubai;
        gangubai.initialize("Gangubai Kathiawadi", 2022, "Sanjay Leela Bhansali", castGangubai, 5, "Drama", 7.0f, 157);
        addMovie(gangubai);

        // Laal Singh Chaddha (2022)
        char castLaal[MAX_CAST][MAX_STRING_LENGTH] = {"Aamir Khan", "Kareena Kapoor", "Naga Chaitanya", "Mona Singh", "Manav Vij"};
        Movie laal;
        laal.initialize("Laal Singh Chaddha", 2022, "Advait Chandan", castLaal, 5, "Drama", 5.3f, 159);
        addMovie(laal);

        // The Kerala Story (2023)
        char castKerala[MAX_CAST][MAX_STRING_LENGTH] = {"Adah Sharma", "Yogita Bihani", "Sonia Balani", "Siddhi Idnani", "Devadarshini"};
        Movie kerala;
        kerala.initialize("The Kerala Story", 2023, "Sudipto Sen", castKerala, 5, "Drama", 7.2f, 138);
        addMovie(kerala);

        // Rocky Aur Rani Kii Prem Kahaani (2023)
        char castRocky[MAX_CAST][MAX_STRING_LENGTH] = {"Ranveer Singh", "Alia Bhatt", "Dharmendra", "Jaya Bachchan", "Shabana Azmi"};
        Movie rocky;
        rocky.initialize("Rocky Aur Rani Kii Prem Kahaani", 2023, "Karan Johar", castRocky, 5, "Romance", 6.5f, 168);
        addMovie(rocky);

        std::cout << "Added 20 recent English and Hindi movies from 2020-2025!" << std::endl;
    }
};

int main()
{
    MovieDatabase database;

    // Try to load database from file first
    bool loadedFromFile = database.loadFromFile();

    // If no file was loaded, initialize with sample data
    if (!loadedFromFile)
    {
        std::cout << "Initializing database with sample data..." << std::endl;
        database.initializeWithSampleData();
    }

    database.runMenu();
    return 0;
}
