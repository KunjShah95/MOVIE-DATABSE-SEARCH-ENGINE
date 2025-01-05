#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

// Movie class definition
class Movie
{
public:
    string title;
    int releaseYear;
    string director;
    vector<string> cast;
    string genre;
    float rating;
    int duration;

    // Constructor
    Movie(string title, int releaseYear, string director, vector<string> cast, string genre, float rating, int duration)
    {
        this->title = title;
        this->releaseYear = releaseYear;
        this->director = director;
        this->cast = cast;
        this->genre = genre;
        this->rating = rating;
        this->duration = duration;
    }

    // Function to display movie details
    void display() const
    {
        cout << "Title: " << title << endl;
        cout << "Release Year: " << releaseYear << endl;
        cout << "Director: " << director << endl;
        cout << "Cast: ";
        for (const auto &actor : cast)
        {
            cout << actor << " ";
        }
        cout << endl;
        cout << "Genre: " << genre << endl;
        cout << "Rating: " << rating << endl;
        cout << "Duration: " << duration << " minutes" << endl;
        cout << "--------------------------------------" << endl;
    }
};

// Function for linear search by title
void linearSearch(const vector<Movie> &database, const string &title)
{
    bool found = false;
    for (const Movie &movie : database)
    {
        if (movie.title.find(title) != string::npos)
        {
            movie.display();
            found = true;
        }
    }
    if (!found)
    {
        cout << "No movies found with title: " << title << endl;
    }
}

// Function for binary search by release year or rating
void binarySearch(vector<Movie> &database, const string &criteria, int year = 0, float rating = 0.0)
{
    if (criteria == "year")
    {
        sort(database.begin(), database.end(), [](const Movie &a, const Movie &b)
             { return a.releaseYear < b.releaseYear; });
        auto it = lower_bound(database.begin(), database.end(), year, [](const Movie &movie, int year)
                              { return movie.releaseYear < year; });
        if (it != database.end() && it->releaseYear == year)
        {
            it->display();
        }
        else
        {
            cout << "No movies found with release year: " << year << endl;
        }
    }
    else if (criteria == "rating")
    {
        sort(database.begin(), database.end(), [](const Movie &a, const Movie &b)
             { return a.rating < b.rating; });
        auto it = lower_bound(database.begin(), database.end(), rating, [](const Movie &movie, float rating)
                              { return movie.rating < rating; });
        if (it != database.end() && it->rating == rating)
        {
            it->display();
        }
        else
        {
            cout << "No movies found with rating: " << rating << endl;
        }
    }
}

// Function to add a new movie
void addMovie(vector<Movie> &database)
{
    string title, director, genre, castMember;
    int releaseYear, duration, castCount;
    float rating;
    vector<string> cast;

    cout << "Enter movie title: ";
    cin.ignore();
    getline(cin, title);

    cout << "Enter release year: ";
    cin >> releaseYear;

    cout << "Enter director: ";
    cin.ignore();
    getline(cin, director);

    cout << "Enter genre: ";
    getline(cin, genre);

    cout << "Enter rating: ";
    cin >> rating;

    cout << "Enter duration (in minutes): ";
    cin >> duration;

    cout << "Enter number of cast members: ";
    cin >> castCount;
    cin.ignore();

    for (int i = 0; i < castCount; i++)
    {
        cout << "Enter cast member " << i + 1 << ": ";
        getline(cin, castMember);
        cast.push_back(castMember);
    }

    database.emplace_back(title, releaseYear, director, cast, genre, rating, duration);
    cout << "Movie added successfully!" << endl;
}

// Function to delete a movie
void deleteMovie(vector<Movie> &database, const string &title)
{
    auto it = remove_if(database.begin(), database.end(), [&](const Movie &movie)
                        { return movie.title == title; });
    if (it != database.end())
    {
        database.erase(it, database.end());
        cout << "Movie deleted successfully!" << endl;
    }
    else
    {
        cout << "Movie not found!" << endl;
    }
}

// Function to view all movies
void viewAllMovies(const vector<Movie> &database)
{
    if (database.empty())
    {
        cout << "No movies in the database." << endl;
        return;
    }

    for (const auto &movie : database)
    {
        movie.display();
    }
}

// Function to search by genre or director
void searchByGenreOrDirector(const vector<Movie> &database, const string &criteria, const string &value)
{
    bool found = false;
    for (const Movie &movie : database)
    {
        if ((criteria == "genre" && movie.genre == value) || (criteria == "director" && movie.director == value))
        {
            movie.display();
            found = true;
        }
    }
    if (!found)
    {
        cout << "No movies found for " << criteria << ": " << value << endl;
    }
}

// Main menu
void menu(vector<Movie> &database)
{
    int choice;
    do
    {
        cout << "\nMovie Database Search System\n";
        cout << "1. Search by Title\n";
        cout << "2. Search by Release Year\n";
        cout << "3. Search by Rating\n";
        cout << "4. Add Movie\n";
        cout << "5. Delete Movie\n";
        cout << "6. View All Movies\n";
        cout << "7. Search by Genre\n";
        cout << "8. Search by Director\n";
        cout << "9. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
        {
            string title;
            cout << "Enter movie title: ";
            cin.ignore();
            getline(cin, title);
            linearSearch(database, title);
            break;
        }
        case 2:
        {
            int year;
            cout << "Enter release year: ";
            cin >> year;
            binarySearch(database, "year", year);
            break;
        }
        case 3:
        {
            float rating;
            cout << "Enter rating: ";
            cin >> rating;
            binarySearch(database, "rating", 0, rating);
            break;
        }
        case 4:
            addMovie(database);
            break;
        case 5:
        {
            string title;
            cout << "Enter movie title to delete: ";
            cin.ignore();
            getline(cin, title);
            deleteMovie(database, title);
            break;
        }
        case 6:
            viewAllMovies(database);
            break;
        case 7:
        {
            string genre;
            cout << "Enter genre: ";
            cin.ignore();
            getline(cin, genre);
            searchByGenreOrDirector(database, "genre", genre);
            break;
        }
        case 8:
        {
            string director;
            cout << "Enter director: ";
            cin.ignore();
            getline(cin, director);
            searchByGenreOrDirector(database, "director", director);
            break;
        }
        case 9:
            cout << "Exiting..." << endl;
            break;
        default:
            cout << "Invalid choice. Please try again." << endl;
        }
    } while (choice != 9);
}

int main()
{
    vector<Movie> database;

    // Adding more movies to the database
    database.push_back(Movie("The Shawshank Redemption", 1994, "Frank Darabont", {"Tim Robbins", "Morgan Freeman", "Bob Gunton"}, "Drama", 9.3, 142));
    database.push_back(Movie("The Godfather", 1972, "Francis Ford Coppola", {"Marlon Brando", "Al Pacino", "James Caan"}, "Crime", 9.2, 175));
    database.push_back(Movie("The Dark Knight", 2008, "Christopher Nolan", {"Christian Bale", "Heath Ledger", "Aaron Eckhart"}, "Action", 9.0, 152));
    database.push_back(Movie("Pulp Fiction", 1994, "Quentin Tarantino", {"John Travolta", "Uma Thurman", "Samuel L. Jackson"}, "Crime", 8.9, 154));
    database.push_back(Movie("Schindler's List", 1993, "Steven Spielberg", {"Liam Neeson", "Ralph Fiennes", "Ben Kingsley"}, "Biography", 9.0, 195));
    database.push_back(Movie("The Lord of the Rings: The Return of the King", 2003, "Peter Jackson", {"Elijah Wood", "Viggo Mortensen", "Ian McKellen"}, "Adventure", 9.0, 201));
    database.push_back(Movie("Inception", 2010, "Christopher Nolan", {"Leonardo DiCaprio", "Joseph Gordon-Levitt", "Elliot Page"}, "Sci-Fi", 8.8, 148));
    database.push_back(Movie("Fight Club", 1999, "David Fincher", {"Brad Pitt", "Edward Norton", "Helena Bonham Carter"}, "Drama", 8.8, 139));
    database.push_back(Movie("Forrest Gump", 1994, "Robert Zemeckis", {"Tom Hanks", "Robin Wright", "Gary Sinise"}, "Drama", 8.8, 142));
    database.push_back(Movie("The Matrix", 1999, "Lana Wachowski, Lilly Wachowski", {"Keanu Reeves", "Laurence Fishburne", "Carrie-Anne Moss"}, "Sci-Fi", 8.7, 136));
    database.push_back(Movie("Goodfellas", 1990, "Martin Scorsese", {"Robert De Niro", "Ray Liotta", "Joe Pesci"}, "Crime", 8.7, 145));
    database.push_back(Movie("The Empire Strikes Back", 1980, "Irvin Kershner", {"Mark Hamill", "Harrison Ford", "Carrie Fisher"}, "Action", 8.7, 124));
    database.push_back(Movie("Interstellar", 2014, "Christopher Nolan", {"Matthew McConaughey", "Anne Hathaway", "Jessica Chastain"}, "Adventure", 8.6, 169));
    database.push_back(Movie("Se7en", 1995, "David Fincher", {"Brad Pitt", "Morgan Freeman", "Gwyneth Paltrow"}, "Crime", 8.6, 127));
    database.push_back(Movie("The Silence of the Lambs", 1991, "Jonathan Demme", {"Jodie Foster", "Anthony Hopkins", "Scott Glenn"}, "Thriller", 8.6, 118));

    menu(database);

    return 0;
}