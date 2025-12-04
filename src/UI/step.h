#pragma once
#ifndef step_h
#define step_h

#define LANE_NUMBER 4

/**
 * @class Step
 * A class that holds all the important functionality of individual steps
 * does not display anything but has information that helps tell the UI
 * how to display that specific step
 * 
 * @author Kiyoko Iuchi-Fung
 * @version 0.1.0
 */
class Step {
public:
    /**
     * Constructor with the index it exists at in the step vector
     * 
     * @param index the index of the step within the step vector
     */
    Step(int index) {
        this->note = 48;
        this->instrument = -1;
        this->fx = 0;
        this->fxAmount = 0;
        this->selected = false;
        this->paramEdit = Step::i;
        this->index = index;
    }

    /**
     * A more verbose constructor that is made for project loading where 
     * steps start with pre assigned values from the file and not default 
     * values
     * 
     * @param instrument the instrument field of the step
     * @param note the note field of the step
     * @param fx the fx field of the step
     * @param fxAmount the fxAmount field of the step
     * @param index the index of the step within the step vector
     */
    Step(int8_t instrument, uint8_t note, uint8_t fx, uint8_t fxAmount, int index) {
        this->note = note;
        this->instrument = instrument;
        this->fx = fx;
        this->fxAmount = fxAmount;
        this->selected = false;
        this->paramEdit = Step::i;
        this->index = index;
    }

    ~Step(){}

    /**
     * param holds all the possibe parameters that can be edited
     */
    enum param {i, n, f, fa};

    /**
     * The actual important data fields
     */
    int8_t  instrument; /**< Instrument to be triggered */
    uint8_t note;       /**< Note to be played */
    uint8_t fx;         /**< Step fx to be applied */
    uint8_t fxAmount;   /**< How much fx applied */

    /**
     * These fields have to do with UI access and display
     */
    bool    selected;   /**< Describes whether the step is selected or not */
    int     index;      /**< Position within the step vector */
    param   paramEdit;  /**< Param to be edited */

    /**
     * Increments paramEdit within some constraints
     * called from within UI to change which parameter is 
     * edited by Increment() and Decrement() functions
     */
    void NextParam() {
        if ((int) paramEdit < 3) paramEdit = (param) ((int) paramEdit + 1);
        else paramEdit = (param) 3;
    }

    /**
     * Decrements paramEdit within some constraints
     * called from within UI to change which parameter is 
     * edited by Increment() and Decrement() functions
     */
    void PrevParam() {
        if ((int) paramEdit > 0) paramEdit = (param) ((int) paramEdit - 1);
        else paramEdit = (param) 0;
    }

    /**
     * Increments the specific parameter selected by paramEdit
     */
    void Increment() {
        if (paramEdit == i) instrument += 1;
        else if (paramEdit == n) note += 1;
        else if (paramEdit == f) fx += 1;
        else if (paramEdit == fa) fxAmount += 1;
    }

    /**
     * Decrements the specific parameter selected by paramEdit
     */
    void Decrement () {
        if (paramEdit == i) instrument = (instrument <= -2) ? -2 : instrument - 1;
        else if (paramEdit == n) note = (note <= 0) ? 0 : note - 1;
        else if (paramEdit == f) fx = (fx <= 0) ? 0 : fx - 1;
        else if (paramEdit == fa) fxAmount = (fxAmount <= 0) ? 0 : fxAmount - 1;
    }

    /**
     * A more verbose constructor that is made for project loading where 
     * steps start with pre assigned values from the file and not default 
     * values
     * 
     * @param instrument the instrument field of the step
     * @param note the note field of the step
     * @param fx the fx field of the step
     * @param fxAmount the fxAmount field of the step
     * @param index the index of the step within the step vector
     */
    void Init(int8_t instrument, uint8_t note, uint8_t fx, uint8_t fxAmount, int index) {
        this->note = note;
        this->instrument = instrument;
        this->fx = fx;
        this->fxAmount = fxAmount;
        this->selected = false;
        this->paramEdit = Step::i;
        this->index = index;
    }
};


/**
 * @brief Nested class to deal with linkedness and individual steps
 * Organizes all lanes into 1 array of LANE_NUMBER steps
 * This acts as a Node inside a circular doubly linked list
 * 
 * @tparam numLanes 
 */
template <uint16_t numLanes>
class StepCluster {
public:
    /**
     * @brief Construct a new Step Cluster object
     * 
     * @param index of the steps within the cluster
     */
    StepCluster(int index) {
        laneNum = numLanes;
        for (int i = 0; i < numLanes; i++) {
            steps[i] = new Step(index);
        }
    }

    /**
     * @brief Construct a new Step Cluster object
     * 
     * @param steps an array of steps that the StepCluster will hold
     */
    StepCluster(Step* steps[numLanes]) {
        laneNum = numLanes;
        this->steps = steps;
    }

    /**
     * @brief Generically construct a new Step Cluster object
     */
    StepCluster() {
        laneNum = numLanes;
    }

    /**
     * @brief Destroy the Step Cluster object
     */
    ~StepCluster() {
        for (int i = 0; i < numLanes; i++) {
            delete steps[i];
        }
    }

    Step*           steps[numLanes];    /**< The array of steps */
    StepCluster*    next;               /**< Pointer to the next step cluster */
    StepCluster*    prev;               /**< Pointer to the previous step cluster */

    uint16_t laneNum;
};

/**
 * Lane class that manages steps like a circular doubly linked list
 * Allows for dynamic memory management without dealing with std::vector.
 * Could load Steps into SDRAM 
 * Works like a circularly linked list so that looping is easy
 * 
 * @author Kiyoko Iuchi-Fung
 * @version 0.1.0
 */

class Pattern {
public:

    

    /**
     * Constructor and destructor
     */
    Pattern() {
        head = nullptr;
        tail = nullptr;
        current = nullptr;
        size = 0;
    }
    ~Pattern(){
        while (size != 0) {
            StepCluster<LANE_NUMBER>* curr = remove();
            delete curr;
        }
    }

     /**
      * @brief Add step to the back of the pattern
      * 
      * @param step pointer to the stepcluster object to be added
      * @return true success
      * @return false failure
      */
    bool push(StepCluster<LANE_NUMBER>* step) {
        if (step == nullptr)
            return false;
        
        if (size == 0) {
            head = step;
            tail = step;
            current = step;
        }

        else {
            // adding to back
            tail->next = step;
            step->prev = tail;

            // updating tail
            tail       = step;

            // maintaining circularity
            head->prev = tail;
            tail->next = head; 
        }

        size++;

        return true;
    }

    /**
     * @brief Add step to the back of the pattern
     * 
     * @param index of the step in the pattern
     * @return true success
     * @return false failure
     */
    bool push(int index) {

        if (index < 0)
            return false;

        StepCluster<LANE_NUMBER>* step = new StepCluster<LANE_NUMBER>(index);

        if (size == 0) {
            head = step;
            tail = step;
            current = step;
        }

        else {
            // adding to back
            tail->next = step;
            step->prev = tail;

            // updating tail
            tail       = step;

            // maintaining circularity
            head->prev = tail;
            tail->next = head; 
        }

        size++;

        return true;

    }

    /**
     * Remove step from the back of the pattern
     * 
     * @note does not free memory of object
     * @return pointer to the removed object
     */
    StepCluster<LANE_NUMBER>* remove() {
        if (size == 0) 
            return nullptr;

        StepCluster<LANE_NUMBER>* n = tail;

        if (current == tail) 
            current = tail->prev;

        // update tail
        tail = tail->prev;

        // maintain circularity
        tail->next = head;
        head->prev = tail;

        size--;

        return n;
    }

    /**
     * Gets the current step of the pattern
     * 
     * @return pointer to the current Step
     */
    StepCluster<LANE_NUMBER>* get_current() {
        return current;
    }

    /**
     * @brief Get the current step object at a specified lane
     * 
     * @param lane the lane of the step to access
     * @return Step* 
     */
    Step* get_current(int lane) {
        return current->steps[lane];
    }

    /**
     * Increments the value of current step
     * @note circularly linked therefore does note check if it reaches the end
     */
    void increment() {
        current = current->next;
    }

    /**
     * Decrements the value of current step
     * @note circularly linked therefore does note check if it reaches the end
     */
    void decrement() {
        current = current->prev;
    }

    /**
     * @brief resets current back to the head
     * 
     */
    void reset() {
        current = head;
    }

    /**
     * Checks if the current step is the tail or not
     * @note This is how to check if we need to move to the next pattern
     * @return bool of whether tail or not
     */
    bool is_tail() {
        return current == tail;
    }

    /**
     * Checks if the current step is the head
     * @return bool of whether head or not
     */
    bool is_head() {
        return current == head;
    }

    /**
     * Gets the size of the pattern
     * @return size of the pattern
     */
    size_t getSize() {
        return size;
    }

    /**
     * @brief Set the Index object
     * 
     * @param index ID of the pattern
     */
    void setIndex(size_t index) {
        this->index = index;
    }

    /**
     * @brief Get the Index object
     * 
     * @return size_t index ID of the pattern
     */
    size_t getIndex() {
        return index;
    }

    /**
     * @brief Get the Tail object
     * 
     * @return StepCluster<LANE_NUMBER>* 
     */
    StepCluster<LANE_NUMBER>* getTail() {
        return tail;
    }

    /**
     * @brief Get the Head object
     * 
     * @return StepCluster<LANE_NUMBER>* 
     */
    StepCluster<LANE_NUMBER>* getHead() {
        return head;
    }

private:
    StepCluster<LANE_NUMBER>*   head;
    StepCluster<LANE_NUMBER>*   tail;
    StepCluster<LANE_NUMBER>*   current;

    size_t size;
    size_t index;
};


#endif