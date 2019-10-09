# ProductionLineSimulation
Inter Process Communication using Shared Memory

READ ME

Η υλοποίηση της 1ης εργασιας αποτελείται από τα αρχεία:

 fork.c: η main συνάρτηση

 queue.c:η υλοποίηση των συναρτήσεων που αφορούν τις ουρές

 functions.c:όλες οι συναρτήσεις σχετικές με διαχείρηση μνήμης, σημαφόρων, κλπ.

 functions.h: το αρχείο κεφαλίδας με τις δηλώσεις των δομών

 makefile: Με την εντολή make γίνεται compile το εκτελέσιμο. Η εκτέλεση πρέπει να έχει
τη μορφή: [./productionLine Y], όπου Y ένας ακέραιος που δίνει ο χρήστης από τη
γραμμή εντολών.


Η εκτέλεση ξεκινάει με την μετατροπή της εισόδου του χρήστη από το command line του
αριθμού εξαρτημάτων που πρέπει να παραχθούν από κάθε μηχάνημα (και άρα τελικών
προϊόντων). Έτσι με τη σειρά φτιάχνονται οι ουρές και τα shared memory segments (τα οποία
αποτελούνται από 1 δομή εξάρτημα και 3 σημαφόρους). Στη συνέχεια φτιάχνονται 3 πίνακες και
2 ακέραιοι τα οποία αντιστοιχούν στα 3 μηχανήματα παραγωγής εξαρτημάτων, στο βαφείο, στα
3 μηχανήματα ελέγχου εξαρτημάτων, στη γραμμή συναρμολόγησης και σε έναν στατικό πίνακα
που αποθηκεύει τα process id’s των προηγούμενων. Με τον τρόπο αυτό η υλοποίηση από δω
και πέρα χωρίζεται στα 4 στάδια παραγωγής.

Για να αποφεύγεται η επανάληψη θα αναλυθεί εδώ μια φορά ο τρόπος λειτουργίας του shared
memory. Για να «ανοίξει» ένα process ένα shared memory segment, πρέπει να ζητήσει access.
Αυτό επιτυγχάνεται με την συνάρτηση down στο σημαφόρο της SM και ανάλογα της
δραστήριοτητας που θέλει να πραγματοποιήσει το process (read/write) τον full/empty
αντιστοιχα. Αναλόγως, αφού διαβάσει ή γράψει, πρέπει να «κλείσει» τη διαμοιραζόμενη μνήμη
για το επόμενο process που θα θέλει να την αξιοποιήσει. Αυτό, προφανώς γίνεται με up των
κατάλληλων σημαφόρων. Ουσιαστικά οι up/down σημαίνουν:
 Up: Ένα process έχει γεμίσει τη μνήμη και τη διαθέτει για ανάγνωση σε κάποιο άλλο.
 Down: Ένα process ζητάει access στη μνήμη για να διαβάσει ή να γράψει.
Έτσι μπορούμε να ξεκινήσουμε την περιγραφή ανά διεργασία:

1. Εξαρτήματα: Για μια πετυχημένη fork()=0 το process φτιάχνει ένα εξάρτημα και του
αναθέτει με τυχαίο seed ένα 4-ψήφιο ID, ένα timestamp και έναν καθοριστικό αριθμό
τύπου εξαρτήματος. Έπειτα περνάει το process αυτό στο shared memory. Αυτή η
διαδικασία επαναλαμβάνεται y φορές από κάθε τύπο εξαρτήματος.
2. Βαφείο: Το βαφείο ανοίγει τη διαμοιραζόμενη μνήμη και διαβάζει από εκεί τα στοιχεία του
κάθε εξαρτήματος. Η δουλειά του βαφείου είναι εικονική, αφού το process δεν καλεί
κάποια άλλη συνάρτηση. Έτσι για το ερώτημα (α) αναμένεται η απάντηση να είναι 
σχετικά μικρή αριθμητικά. Το βαφείο εν συνεχεία παραπέμπει το εξάρτημα, βαμμένο
πλεόν, στον έλεγχο.
3. Έλεγχος: Ο έλεγχος έιναι το πιο ιδιαίτερο κομμάτι της υλοποίησης, καθώς απαιτεί έναν
επιπλέον έλεγχο. Πιο αναλυτικά, μετά το access στη διαμοιραζόμενη μνήμη, ο tester
πρέπει να τσεκάρει αν το εξάρτημα είναι του τύπου που αυτός καλείται να ελέγχει. Στην
περίπτωση λοιπόν που δεν είναι, ο tester πρέπει να κλείσει το shared memory ώστε να
καταφέρει να φτάσει το εξάρτημα στον κατάλληλο έλεγχο. Όταν τον βρει, αντιγράφεται
στον tester και θεωρητικά πραγματοποιείται ο έλεγχος του εξαρτήματος, πριν αυτό
αντιγραφεί στο επόμενο shared memory segment.
4. Συναρμολόγηση: Το στάδιο συναρμολόγησης απαιτεί το μεγαλύτερο κομμάτι
υλοποίησης.Μετα την αντιγραφή των στοιχείων του εξαρτήματος πραγματοποιείται ο
υπολογισμός των 2 δύο μέσων όρων που αναφέρονται στην εκφώνηση. Έτσι αθροίζεται
το συνολικό διάστημα όλων των processes μέσα στο βαφείο και σηματοδοτείται ο
τερματισμός επεξεργασίας κάθε process. Κάθε process στη συνέχεια εισάγεται σε μία
ουρα ανάλογα με τον τύπο του. Όσο υπάρχουν εξαρτήματα και στις 3 ουρές λοιπόν, αυτά
γίνονται pop και υπολογίζεται το τελικό id του προϊόντος. Επιπλέον για το τελικό προϊόν
υπολογίζεται η μικρότερη στιγμή δημιουργίας και η μεγαλύτερη στιγμή εξόδου από τα
εξαρτήματα του. Έτσι τυπώνονται και οι δύο αριθμοί σε second.
Στο τέλος πραγματοποιείται η διαγραφή όλων των δεσμευμένων block μνήμης από δομές μετά
από την διαγραφή των processes.


ΠΑΡΑΤΗΡΗΣΕΙΣ:

1. Τα κλειδιά που χρησιμοποιούνται για τους σημαφόρους και τη διαμοιραζόμενη μνήμη συχνά
δημιουργούν πρόβλημα. Στην περίπτωση αυτή, κάποιος άλλος πόρος του συστήματος
χρησιμοποιεί τη συγκεκριμένη μνήμη (αφού εδώ η διεύθυνση δίνεται με #define) και
μπλοκάρει τη δημιουργία semaphore/ shared memory segment. Αυτό διορθώνεται με μία
απλή αλλαγή της τιμής των κλειδιών στο functions.h.
2. Η υλοποίηση τοποθετείται στο μεγαλύτερο κομμάτι της στο fork.c στη main. Αυτό έχει διττή
αιτιολογία: στο δικό μου μηχάνημα, αυτό καθιστά το πρόγραμμα πιο αποδοτικό. Από την
άλλη, λόγω τεχνικού προβλήματος στο δικό μου υπολογιστή η επεξεργασία πολλών αρχείων
ήταν ιδιαίτερα δύσκολη. Παρ’όλα αυτά, χωρίς να μπορώ να το δοκιμάσω δίνεται ενδεικτικά ο
κώδικας component δομημένος σε συναρτηση ως παράδειγμα.
