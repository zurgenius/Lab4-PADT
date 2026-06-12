#include "menu.h"

#include "array_sequence.h"
#include "lazy/lazy_sequence.h"
#include "list_sequence.h"
#include "streams/file.h"
#include "streams/live_file.h"
#include "tasks/event_statistics.h"
#include "utils.h"

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

const int kMaxSequences = 10;

Sequence<int> *sequences[kMaxSequences];
int sequence_count = 0;

int natural_rule(const Sequence<int> &source) {
    return source.get_count();
}

int plus_one_rule(const Sequence<int> &source) {
    return source.get_last() + 1;
}

int times_ten_rule(const Sequence<int> &source) {
    return source.get_last() + 10;
}

void print_lazy_prefix(const LazySequence<int> &sequence, int count) {
    std::cout << "[";
    for (int index = 0; index < count; index++) {
        if (index > 0) {
            std::cout << ", ";
        }
        std::cout << sequence.get(index);
    }
    std::cout << "]" << std::endl;
}

bool store_sequence(Sequence<int> *sequence) {
    if (sequence_count >= kMaxSequences) {
        return false;
    }
    sequences[sequence_count++] = sequence;
    return true;
}

void replace_if_new(Sequence<int> *&slot, Sequence<int> *result) {
    if (result != slot) {
        delete slot;
        slot = result;
    }
}

int select_sequence(const char *prompt) {
    if (sequence_count == 0) {
        std::cout << "No sequences created" << std::endl;
        return -1;
    }

    std::cout << prompt << std::endl;
    for (int index = 0; index < sequence_count; index++) {
        std::cout << index << ": ";
        utils::print_sequence(sequences[index]);
    }

    std::cout << "Index: ";
    int index = -1;
    utils::read_int(index);
    if (index < 0 || index >= sequence_count) {
        std::cout << "Invalid index" << std::endl;
        return -1;
    }
    return index;
}

Sequence<int> *create_number_sequence(int kind) {
    switch (kind) {
    case 1:
        return new MutableArraySequence<int>();
    case 2:
        return new MutableListSequence<int>();
    case 3:
        return new ImmutableArraySequence<int>();
    case 4:
        return new ImmutableListSequence<int>();
    default:
        return nullptr;
    }
}

void menu_create_sequence() {
    std::cout << "\n=== Create Sequence ===" << std::endl;
    std::cout << "1. MutableArraySequence<int>" << std::endl;
    std::cout << "2. MutableListSequence<int>" << std::endl;
    std::cout << "3. ImmutableArraySequence<int>" << std::endl;
    std::cout << "4. ImmutableListSequence<int>" << std::endl;
    std::cout << "Choice: ";

    int choice = 0;
    utils::read_int(choice);

    Sequence<int> *sequence = create_number_sequence(choice);
    if (sequence == nullptr) {
        std::cout << "Invalid choice" << std::endl;
        return;
    }

    std::cout << "Enter number of elements: ";
    int count = 0;
    utils::read_int(count);

    for (int index = 0; index < count; index++) {
        std::cout << "Element " << index << ": ";
        int value = 0;
        utils::read_int(value);
        Sequence<int> *result = sequence->append(value);
        replace_if_new(sequence, result);
    }

    if (!store_sequence(sequence)) {
        delete sequence;
        std::cout << "No free slots to store sequence" << std::endl;
        return;
    }
    std::cout << "Stored as sequence " << (sequence_count - 1) << std::endl;
}

void menu_add_element() {
    const int index = select_sequence("Select sequence:");
    if (index == -1) {
        return;
    }

    std::cout << "Enter element: ";
    int value = 0;
    utils::read_int(value);

    Sequence<int> *result = sequences[index]->append(value);
    replace_if_new(sequences[index], result);
    std::cout << "Element appended" << std::endl;
}

void menu_print_all() {
    if (sequence_count == 0) {
        std::cout << "No sequences created" << std::endl;
        return;
    }

    std::cout << "\n--- Sequence<int> ---" << std::endl;
    for (int index = 0; index < sequence_count; index++) {
        std::cout << "[" << index << "] ";
        utils::print_sequence(sequences[index]);
    }
}

void menu_get_element() {
    const int sequence_index = select_sequence("Select sequence:");
    if (sequence_index == -1) {
        return;
    }

    std::cout << "Enter element index: ";
    int item_index = 0;
    utils::read_int(item_index);

    try {
        std::cout << "Element: " << (*sequences[sequence_index])[item_index] << std::endl;
    } catch (const std::out_of_range &e) {
        std::cout << "Error: " << e.what() << std::endl;
    }
}

void menu_get_subsequence() {
    const int index = select_sequence("Select sequence:");
    if (index == -1) {
        return;
    }

    std::cout << "Start index: ";
    int start = 0;
    utils::read_int(start);
    std::cout << "End index: ";
    int end = 0;
    utils::read_int(end);

    try {
        Sequence<int> *result = sequences[index]->get_sub_sequence(start, end);
        std::cout << "Result: ";
        utils::print_sequence(result);

        if (!store_sequence(result)) {
            delete result;
            std::cout << "No free slots to store result" << std::endl;
            return;
        }
        std::cout << "Stored as sequence " << (sequence_count - 1) << std::endl;
    } catch (const std::out_of_range &e) {
        std::cout << "Error: " << e.what() << std::endl;
    }
}

void menu_concat() {
    const int left = select_sequence("Select first sequence:");
    if (left == -1) {
        return;
    }
    const int right = select_sequence("Select second sequence:");
    if (right == -1) {
        return;
    }

    Sequence<int> *result = sequences[left]->concat(sequences[right]);
    std::cout << "Result: ";
    utils::print_sequence(result);

    if (!store_sequence(result)) {
        delete result;
        std::cout << "No free slots to store result" << std::endl;
        return;
    }
    std::cout << "Stored as sequence " << (sequence_count - 1) << std::endl;
}

void menu_map() {
    const int index = select_sequence("Select sequence:");
    if (index == -1) {
        return;
    }

    Sequence<int> *result = sequences[index]->map(utils::square);
    std::cout << "Mapped sequence: ";
    utils::print_sequence(result);

    if (result == sequences[index]) {
        std::cout << "Sequence updated in place" << std::endl;
        return;
    }
    if (!store_sequence(result)) {
        delete result;
        std::cout << "No free slots to store result" << std::endl;
        return;
    }
    std::cout << "Stored as sequence " << (sequence_count - 1) << std::endl;
}

void menu_where() {
    const int index = select_sequence("Select sequence:");
    if (index == -1) {
        return;
    }

    Sequence<int> *result = sequences[index]->where(utils::is_positive);
    std::cout << "Filtered sequence: ";
    utils::print_sequence(result);

    if (result == sequences[index]) {
        std::cout << "Sequence updated in place" << std::endl;
        return;
    }
    if (!store_sequence(result)) {
        delete result;
        std::cout << "No free slots to store result" << std::endl;
        return;
    }
    std::cout << "Stored as sequence " << (sequence_count - 1) << std::endl;
}

void menu_reduce() {
    const int index = select_sequence("Select sequence:");
    if (index == -1) {
        return;
    }
    std::cout << "Reduce(sum): " << sequences[index]->reduce(utils::sum, 0) << std::endl;
}

void menu_slice() {
    const int index = select_sequence("Select sequence:");
    if (index == -1) {
        return;
    }

    std::cout << "Start index: ";
    int start = 0;
    utils::read_int(start);
    std::cout << "Delete count: ";
    int count = 0;
    utils::read_int(count);
    std::cout << "Replacement size: ";
    int replacement_size = 0;
    utils::read_int(replacement_size);

    Sequence<int> *replacement = nullptr;
    if (replacement_size > 0) {
        replacement = new MutableArraySequence<int>();
        for (int item = 0; item < replacement_size; item++) {
            std::cout << "Replacement element " << item << ": ";
            int value = 0;
            utils::read_int(value);
            replacement->append(value);
        }
    }

    try {
        Sequence<int> *result = sequences[index]->slice(start, count, replacement);
        std::cout << "Result: ";
        utils::print_sequence(result);

        if (!store_sequence(result)) {
            delete result;
            std::cout << "No free slots to store result" << std::endl;
        } else {
            std::cout << "Stored as sequence " << (sequence_count - 1) << std::endl;
        }
    } catch (const std::exception &e) {
        std::cout << "Error: " << e.what() << std::endl;
    }

    delete replacement;
}

void menu_run_tests() {
    std::cout << "\nRunning tests..." << std::endl;
    const int status = std::system("make tests && ./tests_runner");
    if (status != 0) {
        std::cout << "Test run failed" << std::endl;
    }
}

void menu_lab4_lazy_demo() {
    int zero_data[] = {0};
    int ten_data[] = {10};
    int hundred_data[] = {100};
    MutableArraySequence<int> zero_init(zero_data, 1);
    MutableArraySequence<int> ten_init(ten_data, 1);
    MutableArraySequence<int> hundred_init(hundred_data, 1);

    LazySequence<int> naturals(natural_rule, zero_init);
    LazySequence<int> tens(times_ten_rule, ten_init);
    LazySequence<int> hundreds(plus_one_rule, hundred_init);

    std::cout << "\nNaturals prefix: ";
    print_lazy_prefix(naturals, 10);

    LazySequence<int> *appended = naturals.append(999);
    std::cout << "Append is lazy, prefix after append generator: ";
    print_lazy_prefix(*appended, 10);

    LazySequence<int> *inserted = naturals.insert_at(-1, 0);
    std::cout << "Insert is lazy, prefix after insert generator: ";
    print_lazy_prefix(*inserted, 10);

    const LazySequence<int> *sources[] = {&naturals, &tens, &hundreds};
    LazySequence<int> *interleaved = LazySequence<int>::interleave(sources, 3);
    std::cout << "Interleave three infinite sequences: ";
    print_lazy_prefix(*interleaved, 12);

    LazySequence<int> *concat = naturals.concat(tens);
    std::cout << "Concat length omega count: " << concat->get_length().get_omega_count()
              << ", finite tail: " << concat->get_length().get_finite_count() << std::endl;
    std::cout << "Concat value after first omega: "
              << concat->get(OrdinalIndex::omega_plus(0)) << std::endl;

    delete concat;
    delete interleaved;
    delete inserted;
    delete appended;
}

void menu_lab4_statistics_demo() {
    std::cout << "\nEnter protocol filename: ";
    std::string filename;
    std::cin >> filename;

    try {
        FileLineReadOnlyStream lines(filename);
        OnlineEventStatistics<double> statistics =
            ProtocolStatisticsTask<double>::process(lines);

        std::cout << "Total: " << statistics.get_total_events() << std::endl;
        std::cout << "Start: " << statistics.get_start_events() << std::endl;
        std::cout << "End: " << statistics.get_end_events() << std::endl;
        std::cout << "Measure: " << statistics.get_measure_events() << std::endl;
        std::cout << "Error: " << statistics.get_error_events() << std::endl;
        std::cout << "Unknown: " << statistics.get_unknown_events() << std::endl;

        if (statistics.has_any_measurements()) {
            std::cout << "Min: " << statistics.get_min_measure() << std::endl;
            std::cout << "Max: " << statistics.get_max_measure() << std::endl;
            std::cout << "Average: " << statistics.get_average_measure() << std::endl;
            std::cout << "Variance: " << statistics.get_variance_measure() << std::endl;
        }
    } catch (const std::exception &e) {
        std::cout << "Error: " << e.what() << std::endl;
    }
}

void menu_lab4_live_demo() {
    std::cout << "\nEnter live protocol filename: ";
    std::string filename;
    std::cin >> filename;

    try {
        LiveFileLineReadOnlyStream lines(filename, 20, 100);
        EventReadOnlyStream<double> events(&lines);
        OnlineEventStatistics<double> statistics =
            EventBatchProcessingTask<double>::process(events, 3);

        std::cout << "Live total: " << statistics.get_total_events() << std::endl;
        std::cout << "Live measures: " << statistics.get_measure_events() << std::endl;
        std::cout << "Live errors: " << statistics.get_error_events() << std::endl;
    } catch (const std::exception &e) {
        std::cout << "Error: " << e.what() << std::endl;
    }
}

void menu_lab4() {
    int choice = -1;
    while (choice != 0) {
        std::cout << "\n==== Lab4 Menu ====" << std::endl;
        std::cout << "1. LazySequence demo" << std::endl;
        std::cout << "2. Protocol statistics from file" << std::endl;
        std::cout << "3. Live protocol statistics" << std::endl;
        std::cout << "0. Back" << std::endl;
        std::cout << "Choice: ";
        utils::read_int(choice);

        switch (choice) {
        case 1:
            menu_lab4_lazy_demo();
            break;
        case 2:
            menu_lab4_statistics_demo();
            break;
        case 3:
            menu_lab4_live_demo();
            break;
        case 0:
            break;
        default:
            std::cout << "Invalid choice" << std::endl;
            break;
        }
    }
}

void destroy_all() {
    for (int index = 0; index < sequence_count; index++) {
        delete sequences[index];
        sequences[index] = nullptr;
    }
}

void run_menu() {
    int choice = -1;
    while (choice != 0) {
        std::cout << "\n==== Main Menu ====" << std::endl;
        std::cout << "1. Create sequence" << std::endl;
        std::cout << "2. Append element" << std::endl;
        std::cout << "3. Print all sequences" << std::endl;
        std::cout << "4. Get element" << std::endl;
        std::cout << "5. Get subsequence" << std::endl;
        std::cout << "6. Concat sequences" << std::endl;
        std::cout << "7. Map (square)" << std::endl;
        std::cout << "8. Where (positive)" << std::endl;
        std::cout << "9. Reduce (sum)" << std::endl;
        std::cout << "10. Slice" << std::endl;
        std::cout << "11. Run tests" << std::endl;
        std::cout << "12. Lab4 lazy sequences and streams" << std::endl;
        std::cout << "0. Exit" << std::endl;
        std::cout << "Choice: ";
        utils::read_int(choice);

        switch (choice) {
        case 1:
            menu_create_sequence();
            break;
        case 2:
            menu_add_element();
            break;
        case 3:
            menu_print_all();
            break;
        case 4:
            menu_get_element();
            break;
        case 5:
            menu_get_subsequence();
            break;
        case 6:
            menu_concat();
            break;
        case 7:
            menu_map();
            break;
        case 8:
            menu_where();
            break;
        case 9:
            menu_reduce();
            break;
        case 10:
            menu_slice();
            break;
        case 11:
            menu_run_tests();
            break;
        case 12:
            menu_lab4();
            break;
        case 0:
            break;
        default:
            std::cout << "Invalid choice" << std::endl;
            break;
        }
    }

    destroy_all();
}
