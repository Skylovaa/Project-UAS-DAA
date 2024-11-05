# Define ADP Model for Delivery Scheduling
class DeliveryADP:
    def __init__(self, data, alpha=0.1, gamma=0.9):
        self.data = data  # The delivery data
        self.alpha = alpha  # Learning rate
        self.gamma = gamma  # Discount factor
        self.value_function = {}  # Stores V(s) for each state
        self.policy = {}  # Stores best action for each state
        self.states = self.initialize_states()  # Initialize possible states

    # Step 1: Initialize states from data
    def initialize_states(self): ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        # Define states based on available columns (Location, Traffic, etc.)
        states = self.data[['Orgin', 'Transit', 'Destination', 'Maskapai', 'Daerah Tujuan' , 'Traffic_Condition', 'Weather_Condition', 'Vehicle_Available', 'Personel_Available', 'Additional Notes', 'Runway Occupancy Time', 'Estimated Time']].values
        for state in states:
            state_tuple = tuple(state)
            self.value_function[state_tuple] = 0  # Initialize V(s) to 0
        return states

    # Step 2: Define actions (simplified as 'Proceed' or 'Wait')
    def possible_actions(self, state):
        return ['Proceed', 'Wait']

    # Step 3: Transition function to get the next state based on the current state and action
    def transition(self, state, action):
        if action == 'Proceed':
            # Example transition logic: move to a random next state
            # Select a random row index from the states array
            random_index = np.random.choice(self.states.shape[0])
            # Use the random index to get a 1D state from the 2D states array
            next_state = tuple(self.states[random_index])
        else:
            # If 'Wait', return the current state (indicating delay)
            next_state = state
        return next_state

    # Step 4: Reward function based on action taken
    def reward(self, state, action):
        if action == 'Proceed':
            return 10  # Reward for proceeding
        else:
            return -5  # Penalty for waiting

    # Step 5: Run an ADP iteration (single episode)
    def run_episode(self):
        current_state = tuple(self.states[np.random.randint(len(self.states))])
        total_reward = 0

        for _ in range(10):  # Run for a fixed number of steps
            actions = self.possible_actions(current_state)
            action = np.random.choice(actions)  # Choose an action (explore/exploit could be added here)
            reward = self.reward(current_state, action)
            total_reward += reward

            # Observe next state and update value function
            next_state = self.transition(current_state, action)
            next_state_tuple = tuple(next_state)

            # Update V(s) using ADP value function update
            self.value_function[current_state] += self.alpha * (reward + self.gamma * self.value_function.get(next_state_tuple, 0) - self.value_function[current_state])

            # Update policy to choose the action with the highest estimated value
            best_action = max(actions, key=lambda a: self.value_function.get(next_state_tuple, 0))
            self.policy[current_state] = best_action

            # Move to the next state
            current_state = next_state_tuple

        return total_reward

    # Step 6: Run multiple episodes to improve policy and value function
    def train(self, episodes=100):
        rewards = []
        for episode in range(episodes):
            episode_reward = self.run_episode()
            rewards.append(episode_reward)
        return rewards
